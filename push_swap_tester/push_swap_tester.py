#!/usr/bin/env python3
"""Self-contained tester for the 42 push_swap v1.1 subject and review scale.

The tester does not require checker_linux for correctness tests: it simulates all
11 push_swap operations internally. If checker_linux is present, it is also used
for a small cross-check.
"""

from __future__ import annotations

import argparse
import itertools
import json
import os
import random
import re
import shutil
import subprocess
import sys
import tempfile
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, Sequence

ALLOWED_OPS = (
    "sa", "sb", "ss", "pa", "pb", "ra", "rb", "rr", "rra", "rrb", "rrr"
)
ALLOWED_OP_SET = set(ALLOWED_OPS)
INT_MIN = -(2**31)
INT_MAX = 2**31 - 1


@dataclass
class CommandResult:
    command: list[str]
    returncode: int | None
    stdout: str
    stderr: str
    duration: float
    timed_out: bool = False


@dataclass
class TestResult:
    section: str
    name: str
    status: str
    detail: str = ""
    duration: float = 0.0


class Tester:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.project = Path(args.project).expanduser().resolve()
        self.push_swap = self.project / "push_swap"
        self.checker = self.project / "checker"
        self.results: list[TestResult] = []
        self.random = random.Random(args.seed)
        self.use_color = sys.stdout.isatty() and not args.no_color

    def color(self, text: str, code: str) -> str:
        if not self.use_color:
            return text
        return f"\033[{code}m{text}\033[0m"

    def emit(self, result: TestResult) -> None:
        self.results.append(result)
        style = {"PASS": "32", "FAIL": "31", "WARN": "33", "SKIP": "36"}.get(
            result.status, "0"
        )
        tag = self.color(f"[{result.status}]", style)
        timing = f" ({result.duration:.2f}s)" if result.duration >= 0.01 else ""
        print(f"{tag} {result.section}: {result.name}{timing}")
        if result.detail and (self.args.verbose or result.status in {"FAIL", "WARN"}):
            for line in result.detail.rstrip().splitlines():
                print(f"       {line}")

    def add(
        self,
        section: str,
        name: str,
        status: str,
        detail: str = "",
        duration: float = 0.0,
    ) -> None:
        self.emit(TestResult(section, name, status, detail, duration))

    def run(
        self,
        command: Sequence[str | Path],
        *,
        input_text: str | None = None,
        timeout: float | None = None,
        cwd: Path | None = None,
        env: dict[str, str] | None = None,
    ) -> CommandResult:
        cmd = [str(item) for item in command]
        started = time.monotonic()
        merged_env = os.environ.copy()
        merged_env.update({"LC_ALL": "C", "LANG": "C"})
        if env:
            merged_env.update(env)
        try:
            completed = subprocess.run(
                cmd,
                input=input_text,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                cwd=str(cwd or self.project),
                env=merged_env,
                timeout=timeout or self.args.timeout,
                check=False,
            )
            return CommandResult(
                cmd,
                completed.returncode,
                completed.stdout,
                completed.stderr,
                time.monotonic() - started,
            )
        except subprocess.TimeoutExpired as exc:
            stdout = exc.stdout.decode(errors="replace") if isinstance(exc.stdout, bytes) else (exc.stdout or "")
            stderr = exc.stderr.decode(errors="replace") if isinstance(exc.stderr, bytes) else (exc.stderr or "")
            return CommandResult(
                cmd,
                None,
                stdout,
                stderr,
                time.monotonic() - started,
                timed_out=True,
            )
        except OSError as exc:
            return CommandResult(
                cmd,
                127,
                "",
                str(exc),
                time.monotonic() - started,
            )

    @staticmethod
    def command_detail(result: CommandResult, limit: int = 1200) -> str:
        text = [f"command: {' '.join(result.command)}", f"return code: {result.returncode}"]
        if result.timed_out:
            text.append("timed out")
        if result.stdout:
            text.append("stdout:\n" + result.stdout[:limit])
        if result.stderr:
            text.append("stderr:\n" + result.stderr[:limit])
        return "\n".join(text)

    @staticmethod
    def parse_operations(stdout: str) -> tuple[list[str] | None, str]:
        if stdout == "":
            return [], ""
        if "\r" in stdout:
            return None, "stdout contains carriage returns"
        if not stdout.endswith("\n"):
            return None, "the operation stream does not end with a newline"
        lines = stdout.splitlines()
        invalid = [line for line in lines if line not in ALLOWED_OP_SET]
        if invalid:
            preview = ", ".join(repr(line) for line in invalid[:5])
            return None, f"invalid stdout line(s): {preview}"
        return lines, ""

    @staticmethod
    def execute_operations(values: Sequence[int], operations: Iterable[str]) -> tuple[list[int], list[int]]:
        a = list(values)
        b: list[int] = []

        def swap(stack: list[int]) -> None:
            if len(stack) >= 2:
                stack[0], stack[1] = stack[1], stack[0]

        def rotate(stack: list[int]) -> None:
            if len(stack) >= 2:
                stack.append(stack.pop(0))

        def reverse_rotate(stack: list[int]) -> None:
            if len(stack) >= 2:
                stack.insert(0, stack.pop())

        for op in operations:
            if op == "sa":
                swap(a)
            elif op == "sb":
                swap(b)
            elif op == "ss":
                swap(a)
                swap(b)
            elif op == "pa":
                if b:
                    a.insert(0, b.pop(0))
            elif op == "pb":
                if a:
                    b.insert(0, a.pop(0))
            elif op == "ra":
                rotate(a)
            elif op == "rb":
                rotate(b)
            elif op == "rr":
                rotate(a)
                rotate(b)
            elif op == "rra":
                reverse_rotate(a)
            elif op == "rrb":
                reverse_rotate(b)
            elif op == "rrr":
                reverse_rotate(a)
                reverse_rotate(b)
            else:
                raise ValueError(f"unknown operation: {op}")
        return a, b

    @staticmethod
    def disorder(values: Sequence[int]) -> float:
        pairs = len(values) * (len(values) - 1) // 2
        if pairs == 0:
            return 0.0
        mistakes = sum(
            1
            for i in range(len(values))
            for j in range(i + 1, len(values))
            if values[i] > values[j]
        )
        return mistakes / pairs

    def check_project_directory(self) -> bool:
        if not self.project.is_dir():
            self.add("setup", "project directory", "FAIL", f"not a directory: {self.project}")
            return False
        self.add("setup", "project directory", "PASS", str(self.project))
        return True

    def check_makefile_text(self) -> None:
        path = self.project / "Makefile"
        if not path.is_file():
            self.add("build", "Makefile exists", "FAIL")
            return
        text = path.read_text(errors="replace")
        missing: list[str] = []
        if not re.search(r"(?m)^\s*NAME\s*[:?+]?=", text):
            missing.append("NAME variable")
        for target in ("all", "clean", "fclean", "re"):
            if not re.search(rf"(?m)^\s*{target}\s*:", text):
                missing.append(f"{target} target")
        for flag in ("-Wall", "-Wextra", "-Werror"):
            if flag not in text:
                missing.append(flag)
        if missing:
            self.add("build", "Makefile structure", "FAIL", "missing: " + ", ".join(missing))
        else:
            self.add("build", "Makefile structure", "PASS")

    def check_build(self) -> bool:
        if self.args.skip_build:
            if self.push_swap.is_file() and os.access(self.push_swap, os.X_OK):
                self.add("build", "compilation", "SKIP", "--skip-build; existing executable will be used")
                return True
            self.add("build", "compilation", "FAIL", "--skip-build was used but ./push_swap is unavailable")
            return False

        self.check_makefile_text()
        commands = (("make fclean", ["make", "fclean"]), ("make", ["make"]))
        total = 0.0
        for name, cmd in commands:
            result = self.run(cmd, timeout=self.args.build_timeout)
            total += result.duration
            if result.timed_out or result.returncode != 0:
                self.add("build", name, "FAIL", self.command_detail(result), result.duration)
                return False
            self.add("build", name, "PASS", duration=result.duration)

        if not self.push_swap.is_file() or not os.access(self.push_swap, os.X_OK):
            self.add("build", "push_swap executable", "FAIL", "make did not create an executable ./push_swap")
            return False
        self.add("build", "push_swap executable", "PASS")

        before = self.push_swap.stat().st_mtime_ns
        time.sleep(0.05)
        result = self.run(["make"], timeout=self.args.build_timeout)
        after = self.push_swap.stat().st_mtime_ns if self.push_swap.exists() else -1
        if result.returncode != 0 or result.timed_out:
            self.add("build", "second make", "FAIL", self.command_detail(result), result.duration)
            return False
        if before != after:
            self.add("build", "unnecessary relinking", "FAIL", "push_swap modification time changed on a second make", result.duration)
        else:
            self.add("build", "unnecessary relinking", "PASS", duration=result.duration)

        clean = self.run(["make", "clean"], timeout=self.args.build_timeout)
        if clean.returncode != 0 or clean.timed_out:
            self.add("build", "make clean", "FAIL", self.command_detail(clean), clean.duration)
        elif not self.push_swap.exists():
            self.add("build", "make clean", "FAIL", "make clean removed the final executable", clean.duration)
        else:
            self.add("build", "make clean", "PASS", duration=clean.duration)

        fclean = self.run(["make", "fclean"], timeout=self.args.build_timeout)
        if fclean.returncode != 0 or fclean.timed_out:
            self.add("build", "make fclean", "FAIL", self.command_detail(fclean), fclean.duration)
        elif self.push_swap.exists():
            self.add("build", "make fclean", "FAIL", "./push_swap still exists", fclean.duration)
        else:
            self.add("build", "make fclean", "PASS", duration=fclean.duration)

        rebuild = self.run(["make", "re"], timeout=self.args.build_timeout)
        if rebuild.returncode != 0 or rebuild.timed_out:
            self.add("build", "make re", "FAIL", self.command_detail(rebuild), rebuild.duration)
            return False
        if not self.push_swap.is_file() or not os.access(self.push_swap, os.X_OK):
            self.add("build", "make re", "FAIL", "./push_swap was not recreated", rebuild.duration)
            return False
        self.add("build", "make re", "PASS", duration=rebuild.duration)
        return True

    def check_norminette(self) -> None:
        if self.args.skip_norm:
            self.add("norm", "norminette", "SKIP", "--skip-norm")
            return
        executable = shutil.which("norminette")
        if not executable:
            self.add("norm", "norminette", "SKIP", "norminette is not installed or not in PATH")
            return
        result = self.run([executable, str(self.project)], timeout=self.args.build_timeout)
        combined = result.stdout + result.stderr
        has_error = result.returncode != 0 or bool(re.search(r"\bError!", combined))
        if result.timed_out or has_error:
            self.add("norm", "norminette", "FAIL", self.command_detail(result), result.duration)
        else:
            self.add("norm", "norminette", "PASS", duration=result.duration)

    def check_readme(self) -> None:
        path = self.project / "README.md"
        if not path.is_file():
            self.add("README", "README.md exists", "FAIL")
            return
        text = path.read_text(errors="replace")
        lines = text.splitlines()
        self.add("README", "README.md exists", "PASS")
        first = lines[0].strip() if lines else ""
        match = re.fullmatch(
            r"([*_])This project has been created as part of the 42 curriculum by "
            r"([A-Za-z0-9_-]+(?:\s*,\s*[A-Za-z0-9_-]+)*)\.\1",
            first,
        )
        if not match:
            self.add(
                "README",
                "required first line",
                "FAIL",
                "expected italic text: This project has been created as part of the 42 curriculum by <login1>, <login2>.",
            )
            logins: list[str] = []
        else:
            logins = [item.strip() for item in match.group(2).split(",")]
            self.add("README", "required first line", "PASS", ", ".join(logins))

        if logins:
            if len(logins) == 2:
                self.add("group", "exactly two logins in README first line", "PASS", ", ".join(logins))
            else:
                self.add("group", "exactly two logins in README first line", "FAIL", f"found {len(logins)}: {', '.join(logins)}")

        headings = {
            "Description": ("description", "説明"),
            "Instructions": ("instructions", "手順"),
            "Resources": ("resources", "リソース"),
        }
        for display, candidates in headings.items():
            found = any(
                re.search(rf"(?im)^\s*#+\s*{re.escape(candidate)}\b", text)
                for candidate in candidates
            )
            self.add("README", f"{display} section", "PASS" if found else "FAIL")

        lower = text.lower()
        ai_ok = "ai" in lower and any(word in lower for word in ("used", "usage", "利用", "使用"))
        self.add("README", "AI usage description", "PASS" if ai_ok else "FAIL")

        algorithm_terms = {
            "simple O(n^2)": ("simple", "n^2", "n²"),
            "medium O(n sqrt(n))": ("medium", "sqrt", "√"),
            "complex O(n log n)": ("complex", "log"),
            "adaptive": ("adaptive", "disorder"),
        }
        for display, terms in algorithm_terms.items():
            if display == "simple O(n^2)":
                found = "simple" in lower and ("n^2" in lower or "n²" in lower or "o(n2" in lower)
            elif display == "medium O(n sqrt(n))":
                found = "medium" in lower and ("sqrt" in lower or "√" in text)
            elif display == "complex O(n log n)":
                found = "complex" in lower and "log" in lower
            else:
                found = "adaptive" in lower and ("disorder" in lower or "無秩序" in text)
            self.add("README", f"algorithm explanation: {display}", "PASS" if found else "FAIL")

        contribution_ok = bool(re.search(r"(?im)^\s*#+\s*(contributions?|貢献)", text))
        self.add("README", "contributions section", "PASS" if contribution_ok else "FAIL")

        if (self.project / ".git").exists():
            result = self.run(["git", "shortlog", "-sne", "--all"], timeout=10)
            authors = [line.strip() for line in result.stdout.splitlines() if line.strip()]
            if result.returncode != 0:
                self.add("group", "Git contributors", "WARN", self.command_detail(result))
            elif len(authors) == 2:
                self.add("group", "Git contributors", "PASS", "\n".join(authors))
            else:
                self.add(
                    "group",
                    "Git contributors",
                    "WARN",
                    f"git shortlog reports {len(authors)} author(s); verify the two registered learners manually\n" + "\n".join(authors[:10]),
                )
        else:
            self.add("group", "Git contributors", "SKIP", "project directory is not a Git working tree")

    def valid_case(
        self,
        section: str,
        name: str,
        values: Sequence[int],
        *,
        flags: Sequence[str] = (),
        max_ops: int | None = None,
        require_no_output: bool = False,
        timeout: float | None = None,
    ) -> tuple[bool, int | None, str]:
        result = self.run(
            [self.push_swap, *flags, *(str(value) for value in values)],
            timeout=timeout,
        )
        if result.timed_out:
            self.add(section, name, "FAIL", self.command_detail(result), result.duration)
            return False, None, "timeout"
        if result.returncode is None or result.returncode < 0:
            self.add(section, name, "FAIL", self.command_detail(result), result.duration)
            return False, None, "crash"
        if result.returncode != 0:
            self.add(section, name, "FAIL", self.command_detail(result), result.duration)
            return False, None, "non-zero exit"
        if result.stderr != "":
            self.add(section, name, "FAIL", "unexpected stderr:\n" + result.stderr[:1000], result.duration)
            return False, None, "stderr"
        operations, error = self.parse_operations(result.stdout)
        if operations is None:
            self.add(section, name, "FAIL", error + "\n" + self.command_detail(result), result.duration)
            return False, None, "invalid operations"
        if require_no_output and operations:
            self.add(section, name, "FAIL", f"expected 0 operations, got {len(operations)}", result.duration)
            return False, len(operations), "unexpected operations"
        final_a, final_b = self.execute_operations(values, operations)
        if final_a != sorted(values) or final_b:
            self.add(
                section,
                name,
                "FAIL",
                f"not sorted after {len(operations)} operation(s)\nfinal a: {final_a[:20]}\nfinal b: {final_b[:20]}",
                result.duration,
            )
            return False, len(operations), "not sorted"
        if max_ops is not None and len(operations) > max_ops:
            self.add(
                section,
                name,
                "FAIL",
                f"correctly sorted, but used {len(operations)} operations (limit: {max_ops})",
                result.duration,
            )
            return False, len(operations), "operation limit"
        detail = f"{len(operations)} operation(s)"
        self.add(section, name, "PASS", detail, result.duration)
        return True, len(operations), ""

    def check_errors(self) -> None:
        cases: list[tuple[str, list[str]]] = [
            ("non-numeric argument", ["one"]),
            ("duplicate values", ["3", "2", "3"]),
            ("greater than INT_MAX", [str(INT_MAX + 1)]),
            ("less than INT_MIN", [str(INT_MIN - 1)]),
            ("empty argument", [""]),
            ("sign only", ["+"]),
        ]
        for name, argv in cases:
            result = self.run([self.push_swap, *argv])
            if result.timed_out or result.returncode is None or result.returncode < 0:
                self.add("errors", name, "FAIL", self.command_detail(result), result.duration)
                continue
            if result.stdout == "" and result.stderr == "Error\n":
                self.add("errors", name, "PASS", duration=result.duration)
            else:
                self.add("errors", name, "FAIL", self.command_detail(result), result.duration)

        result = self.run([self.push_swap])
        if result.timed_out or result.returncode is None or result.returncode < 0:
            self.add("errors", "no arguments", "FAIL", self.command_detail(result), result.duration)
        elif result.stdout == "" and result.stderr == "":
            self.add("errors", "no arguments", "PASS", duration=result.duration)
        else:
            self.add("errors", "no arguments", "FAIL", self.command_detail(result), result.duration)

    def check_strategy_flags(self) -> None:
        values = [5, 4, 3, 2, 1]
        for flag in ("--simple", "--medium", "--complex", "--adaptive"):
            self.valid_case("strategies", flag, values, flags=[flag])
        self.valid_case("strategies", "default adaptive behavior", values)

        matrix_values = {
            "one value": [42],
            "low disorder": [1, 2, 3, 5, 4],
            "medium disorder": [3, 2, 1, 4, 5],
            "mixed 20": self.random.sample(range(-100, 101), 20),
        }
        failures: list[str] = []
        counts: list[str] = []
        for label, sample in matrix_values.items():
            for flag in ("--simple", "--medium", "--complex", "--adaptive"):
                result = self.run([self.push_swap, flag, *(str(v) for v in sample)], timeout=10)
                ops, error = self.parse_operations(result.stdout)
                ok = (
                    not result.timed_out
                    and result.returncode == 0
                    and result.stderr == ""
                    and ops is not None
                )
                if ok:
                    final_a, final_b = self.execute_operations(sample, ops)
                    ok = final_a == sorted(sample) and not final_b
                if not ok:
                    failures.append(f"{flag}, {label}: {error or 'execution/sort failure'}")
                elif ops is not None:
                    counts.append(f"{flag} {label}={len(ops)}")
        if failures:
            self.add("strategies", "all selectors across input regimes", "FAIL", "\n".join(failures[:20]))
        else:
            self.add("strategies", "all selectors across input regimes", "PASS", ", ".join(counts))

    def check_sorted_inputs(self) -> None:
        cases = ([42], [2, 3], [0, 1, 2, 3], list(range(10)))
        for values in cases:
            self.valid_case(
                "identity",
                "already sorted: " + " ".join(map(str, values)),
                values,
                require_no_output=True,
            )

    def check_small_inputs(self) -> None:
        counts: dict[tuple[int, ...], int] = {}
        failed: list[str] = []
        max_count = -1
        for permutation in itertools.permutations((0, 1, 2)):
            result = self.run([self.push_swap, *(str(v) for v in permutation)])
            ops, error = self.parse_operations(result.stdout)
            ok = result.returncode == 0 and result.stderr == "" and ops is not None
            if ok:
                final_a, final_b = self.execute_operations(permutation, ops)
                ok = final_a == sorted(permutation) and not final_b and len(ops) <= 5
            if not ok:
                failed.append(f"{permutation}: {error or 'incorrect result'}")
            elif ops is not None:
                counts[permutation] = len(ops)
                max_count = max(max_count, len(ops))
        if failed:
            self.add("small inputs", "all 3-number permutations", "FAIL", "\n".join(failed))
        else:
            detail = f"all 6 permutations sorted; maximum {max_count} operations"
            canonical = counts.get((2, 1, 0))
            if canonical is not None:
                detail += f"; 2 1 0 used {canonical}"
            self.add("small inputs", "all 3-number permutations", "PASS", detail)

    def check_five_inputs(self) -> None:
        cases = (
            ("review case", [1, 5, 2, 4, 3]),
            ("example 1", [5, 1, 4, 2, 3]),
            ("example 2", [3, 5, 1, 4, 2]),
        )
        for name, values in cases:
            self.valid_case("five inputs", name, values, max_ops=15)

        if self.args.exhaustive_five:
            max_ops = -1
            worst: tuple[int, ...] | None = None
            failures: list[str] = []
            for permutation in itertools.permutations(range(5)):
                result = self.run([self.push_swap, *(str(v) for v in permutation)])
                ops, error = self.parse_operations(result.stdout)
                ok = result.returncode == 0 and result.stderr == "" and ops is not None
                if ok:
                    final_a, final_b = self.execute_operations(permutation, ops)
                    ok = final_a == sorted(permutation) and not final_b
                if not ok:
                    failures.append(f"{permutation}: {error or 'incorrect result'}")
                    if len(failures) >= 10:
                        break
                elif ops is not None and len(ops) > max_ops:
                    max_ops = len(ops)
                    worst = permutation
            if failures:
                self.add("five inputs", "all 120 permutations", "FAIL", "\n".join(failures))
            else:
                status = "PASS" if max_ops <= 15 else "WARN"
                self.add(
                    "five inputs",
                    "all 120 permutations",
                    status,
                    f"maximum {max_ops} operations at {worst}; review guideline is 15 or fewer",
                )

    @staticmethod
    def closest_percentage(stderr: str, expected: float) -> float | None:
        values = [float(x) for x in re.findall(r"(?<![\w.])(\d+(?:\.\d+)?)\s*%", stderr)]
        if not values:
            return None
        return min(values, key=lambda value: abs(value - expected))

    def check_benchmark(self) -> None:
        cases = (
            ("sorted / 0% disorder", [1, 2, 3, 4, 5]),
            ("low disorder", [1, 2, 3, 5, 4]),
            ("medium disorder", [3, 2, 1, 4, 5]),
            ("reverse / 100% disorder", [5, 4, 3, 2, 1]),
        )
        for label, values in cases:
            expected = self.disorder(values) * 100.0
            result = self.run(
                [self.push_swap, "--bench", "--adaptive", *(str(v) for v in values)]
            )
            ops, op_error = self.parse_operations(result.stdout)
            errors: list[str] = []
            if result.timed_out or result.returncode != 0:
                errors.append("execution failed")
            if ops is None:
                errors.append(op_error)
            else:
                final_a, final_b = self.execute_operations(values, ops)
                if final_a != sorted(values) or final_b:
                    errors.append("stdout operation stream does not sort")
            if not result.stderr:
                errors.append("benchmark stderr is empty")
            reported = self.closest_percentage(result.stderr, expected)
            if reported is None:
                errors.append("no percentage found in stderr")
            elif abs(reported - expected) > 0.11:
                errors.append(f"reported disorder {reported:.2f}% differs from expected {expected:.2f}%")
            lower = result.stderr.lower()
            if "adaptive" not in lower:
                errors.append("strategy name 'adaptive' not found in stderr")
            if not ("o(" in lower or "complex" in lower or "sqrt" in lower or "√" in result.stderr):
                errors.append("complexity class not found in stderr")
            if not re.search(r"(?i)total[^0-9]{0,40}\d+", result.stderr):
                errors.append("total operation count not found in stderr")
            missing_ops = [op for op in ALLOWED_OPS if not re.search(rf"(?<!\w){op}(?!\w)", result.stderr)]
            if missing_ops:
                errors.append("per-operation counts missing for: " + ", ".join(missing_ops))
            if errors:
                self.add("benchmark", label, "FAIL", "\n".join(errors) + "\nstderr:\n" + result.stderr[:1600], result.duration)
            else:
                self.add(
                    "benchmark",
                    label,
                    "PASS",
                    f"expected disorder {expected:.2f}%, reported {reported:.2f}%, operations {len(ops or [])}",
                    result.duration,
                )

        normal = self.run([self.push_swap, "3", "2", "1"])
        if normal.stderr == "":
            self.add("benchmark", "metrics absent without --bench", "PASS", duration=normal.duration)
        else:
            self.add("benchmark", "metrics absent without --bench", "FAIL", "unexpected stderr:\n" + normal.stderr[:1000], normal.duration)

        simple = self.run([self.push_swap, "--bench", "--simple", "5", "4", "3", "2", "1"])
        if simple.returncode == 0 and "simple" in simple.stderr.lower():
            self.add("benchmark", "forced strategy is reported", "PASS", duration=simple.duration)
        else:
            self.add("benchmark", "forced strategy is reported", "FAIL", self.command_detail(simple), simple.duration)

    def classify_performance(self, n: int, count: int) -> str:
        if n == 100:
            if count < 700:
                return "excellent"
            if count < 1500:
                return "good"
            if count < 2000:
                return "pass"
            return "fail"
        if count < 5500:
            return "excellent"
        if count < 8000:
            return "good"
        if count < 12000:
            return "pass"
        return "fail"

    def performance_runs(self, n: int, runs: int, population_end: int) -> None:
        limit = 2000 if n == 100 else 12000
        counts: list[int] = []
        failures: list[str] = []
        total_duration = 0.0
        for index in range(1, runs + 1):
            values = self.random.sample(range(1, population_end + 1), n)
            result = self.run(
                [self.push_swap, *(str(v) for v in values)],
                timeout=self.args.performance_timeout,
            )
            total_duration += result.duration
            ops, error = self.parse_operations(result.stdout)
            ok = result.returncode == 0 and not result.timed_out and result.stderr == "" and ops is not None
            if ok:
                final_a, final_b = self.execute_operations(values, ops)
                ok = final_a == sorted(values) and not final_b
            if not ok:
                failures.append(f"run {index}: {error or 'execution/sort failure'}")
                continue
            assert ops is not None
            count = len(ops)
            counts.append(count)
            if count >= limit:
                failures.append(f"run {index}: {count} operations (must be < {limit})")
        if failures:
            detail = "\n".join(failures)
            if counts:
                detail += f"\ncounts: {counts}"
            self.add("performance", f"{n} random numbers x{runs}", "FAIL", detail, total_duration)
        else:
            worst = max(counts)
            average = sum(counts) / len(counts)
            level = self.classify_performance(n, worst)
            self.add(
                "performance",
                f"{n} random numbers x{runs}",
                "PASS",
                f"counts={counts}; average={average:.1f}; worst={worst} ({level})",
                total_duration,
            )

    def check_strategy_comparison(self) -> None:
        values = self.random.sample(range(1, 201), 50)
        counts: dict[str, int] = {}
        failures: list[str] = []
        for flag in ("--simple", "--medium", "--complex", "--adaptive"):
            result = self.run(
                [self.push_swap, flag, *(str(v) for v in values)],
                timeout=self.args.performance_timeout,
            )
            ops, error = self.parse_operations(result.stdout)
            ok = result.returncode == 0 and result.stderr == "" and ops is not None
            if ok:
                final_a, final_b = self.execute_operations(values, ops)
                ok = final_a == sorted(values) and not final_b
            if not ok:
                failures.append(f"{flag}: {error or 'execution/sort failure'}")
            else:
                assert ops is not None
                counts[flag] = len(ops)
        if failures:
            self.add("strategies", "same 50-number input comparison", "FAIL", "\n".join(failures))
            return
        status = "PASS"
        note = ""
        if counts["--complex"] >= counts["--simple"]:
            status = "WARN"
            note = "; complex did not use fewer operations than simple on this sample"
        self.add("strategies", "same 50-number input comparison", status, f"{counts}{note}")

    def check_provided_checker(self) -> None:
        candidates = [self.project / "checker_linux", self.project / "checker_Mac"]
        provided = next((path for path in candidates if path.is_file() and os.access(path, os.X_OK)), None)
        if provided is None:
            self.add("checker cross-check", "provided checker", "SKIP", "checker_linux/checker_Mac not found")
            return
        values = self.random.sample(range(1, 101), 20)
        ps = self.run([self.push_swap, *(str(v) for v in values)])
        ops, error = self.parse_operations(ps.stdout)
        if ps.returncode != 0 or ps.stderr != "" or ops is None:
            self.add("checker cross-check", "provided checker", "FAIL", error or self.command_detail(ps))
            return
        result = self.run([provided, *(str(v) for v in values)], input_text=ps.stdout)
        if result.returncode == 0 and result.stdout == "OK\n":
            self.add("checker cross-check", provided.name, "PASS", duration=result.duration)
        else:
            self.add("checker cross-check", provided.name, "WARN", self.command_detail(result), result.duration)

    def check_valgrind(self) -> None:
        if self.args.skip_valgrind:
            self.add("memory", "valgrind", "SKIP", "--skip-valgrind")
            return
        valgrind = shutil.which("valgrind")
        if not valgrind:
            self.add("memory", "valgrind", "SKIP", "valgrind is not installed or not in PATH")
            return
        cases = (("valid input", ["3", "2", "1"]), ("error input", ["3", "2", "3"]))
        for name, argv in cases:
            with tempfile.NamedTemporaryFile(prefix="push_swap_valgrind_", delete=False) as temp:
                log_path = temp.name
            try:
                result = self.run(
                    [
                        valgrind,
                        "--leak-check=full",
                        "--show-leak-kinds=definite,indirect",
                        "--errors-for-leak-kinds=definite,indirect",
                        "--error-exitcode=101",
                        f"--log-file={log_path}",
                        self.push_swap,
                        *argv,
                    ],
                    timeout=self.args.valgrind_timeout,
                )
                log = Path(log_path).read_text(errors="replace") if Path(log_path).exists() else ""
                if result.timed_out or result.returncode == 101 or re.search(r"ERROR SUMMARY: [1-9]", log):
                    self.add("memory", f"valgrind {name}", "FAIL", log[-2000:] or self.command_detail(result), result.duration)
                elif result.returncode is not None and result.returncode < 0:
                    self.add("memory", f"valgrind {name}", "FAIL", self.command_detail(result), result.duration)
                else:
                    self.add("memory", f"valgrind {name}", "PASS", duration=result.duration)
            finally:
                Path(log_path).unlink(missing_ok=True)

    def build_bonus(self, strict: bool) -> bool:
        result = self.run(["make", "bonus"], timeout=self.args.build_timeout)
        if result.returncode != 0 or result.timed_out:
            self.add("bonus", "make bonus", "FAIL" if strict else "SKIP", self.command_detail(result), result.duration)
            return False
        if not self.checker.is_file() or not os.access(self.checker, os.X_OK):
            self.add("bonus", "checker executable", "FAIL" if strict else "SKIP", "make bonus did not create executable ./checker")
            return False
        self.add("bonus", "make bonus", "PASS", duration=result.duration)
        return True

    def checker_error_case(self, name: str, argv: Sequence[str], stdin: str = "") -> None:
        result = self.run([self.checker, *argv], input_text=stdin)
        if result.timed_out or result.returncode is None or result.returncode < 0:
            self.add("bonus errors", name, "FAIL", self.command_detail(result), result.duration)
        elif result.stdout == "" and result.stderr == "Error\n":
            self.add("bonus errors", name, "PASS", duration=result.duration)
        else:
            self.add("bonus errors", name, "FAIL", self.command_detail(result), result.duration)

    def check_bonus(self, strict: bool) -> None:
        if not self.build_bonus(strict):
            return
        for name, argv in (
            ("non-numeric", ["one"]),
            ("duplicate", ["3", "2", "3"]),
            ("greater than INT_MAX", [str(INT_MAX + 1)]),
            ("empty argument", [""]),
        ):
            self.checker_error_case(name, argv)
        self.checker_error_case("invalid instruction", ["3", "2", "1"], "not_an_op\n")

        no_args = self.run([self.checker], input_text="")
        if no_args.stdout == "" and no_args.stderr == "" and not no_args.timed_out:
            self.add("bonus errors", "no arguments", "PASS", duration=no_args.duration)
        else:
            self.add("bonus errors", "no arguments", "FAIL", self.command_detail(no_args), no_args.duration)

        ko = self.run(
            [self.checker, "0", "9", "1", "8", "2", "7", "3", "6", "4", "5"],
            input_text="sa\npb\nrrr\n",
        )
        if ko.stdout == "KO\n" and ko.stderr == "":
            self.add("bonus checker", "known KO sequence", "PASS", duration=ko.duration)
        else:
            self.add("bonus checker", "known KO sequence", "FAIL", self.command_detail(ko), ko.duration)

        ok_sorted = self.run([self.checker, "0", "1", "2"], input_text="")
        if ok_sorted.stdout == "OK\n" and ok_sorted.stderr == "":
            self.add("bonus checker", "already sorted with EOF", "PASS", duration=ok_sorted.duration)
        else:
            self.add("bonus checker", "already sorted with EOF", "FAIL", self.command_detail(ok_sorted), ok_sorted.duration)

        known_ops = "pb\nra\npb\nra\nsa\nra\npa\npa\n"
        ok_known = self.run([self.checker, "0", "9", "1", "8", "2"], input_text=known_ops)
        if ok_known.stdout == "OK\n" and ok_known.stderr == "":
            self.add("bonus checker", "known OK sequence", "PASS", duration=ok_known.duration)
        else:
            self.add("bonus checker", "known OK sequence", "FAIL", self.command_detail(ok_known), ok_known.duration)

        values = self.random.sample(range(-100, 101), 25)
        ps = self.run([self.push_swap, *(str(v) for v in values)])
        integration = self.run([self.checker, *(str(v) for v in values)], input_text=ps.stdout)
        if ps.returncode == 0 and ps.stderr == "" and integration.stdout == "OK\n" and integration.stderr == "":
            self.add("bonus checker", "push_swap integration", "PASS", duration=ps.duration + integration.duration)
        else:
            self.add(
                "bonus checker",
                "push_swap integration",
                "FAIL",
                "push_swap:\n" + self.command_detail(ps) + "\nchecker:\n" + self.command_detail(integration),
                ps.duration + integration.duration,
            )

    def check_live_count_only(self) -> None:
        cases = (([3, 2, 1], None), ([1, 2, 3], 0))
        for values, expected in cases:
            result = self.run([self.push_swap, "--count-only", *(str(v) for v in values)])
            match = re.fullmatch(r"(\d+)\n", result.stdout)
            if result.returncode != 0 or result.stderr != "" or not match:
                self.add("live coding", "--count-only " + " ".join(map(str, values)), "FAIL", self.command_detail(result), result.duration)
                continue
            count = int(match.group(1))
            if expected is not None and count != expected:
                self.add("live coding", "--count-only " + " ".join(map(str, values)), "FAIL", f"expected {expected}, got {count}", result.duration)
            else:
                self.add("live coding", "--count-only " + " ".join(map(str, values)), "PASS", f"reported {count}", result.duration)

    def print_manual_checklist(self) -> None:
        print("\nManual defense checks (not reliably automatable):")
        print("  - Both learners are present and can explain every part of the code.")
        print("  - Both learners can justify simple, medium, complex, and adaptive complexity in push_swap operations.")
        print("  - Adaptive thresholds and internal methods are documented and understood.")
        print("  - No global variables or forbidden external functions are used.")
        print("  - Git/intra shows exactly two registered learners and meaningful contributions.")
        print("  - During the live-coding section, --count-only is implemented and demonstrated within 10 minutes.")

    def save_json(self) -> None:
        if not self.args.json_report:
            return
        path = Path(self.args.json_report).expanduser().resolve()
        payload = {
            "project": str(self.project),
            "mode": self.args.mode,
            "seed": self.args.seed,
            "results": [asdict(result) for result in self.results],
            "summary": self.summary_counts(),
        }
        path.write_text(json.dumps(payload, indent=2, ensure_ascii=False) + "\n")
        print(f"JSON report: {path}")

    def summary_counts(self) -> dict[str, int]:
        counts = {status: 0 for status in ("PASS", "FAIL", "WARN", "SKIP")}
        for result in self.results:
            counts[result.status] = counts.get(result.status, 0) + 1
        return counts

    def run_all(self) -> int:
        if not self.check_project_directory():
            return 2

        mode = self.args.mode
        should_build = mode in {"quick", "mandatory", "performance", "bonus", "all"}
        built = self.check_build() if should_build else self.push_swap.exists()
        if not built:
            self.print_summary()
            self.save_json()
            return 1

        if mode in {"quick", "mandatory", "all"}:
            self.check_norminette()
            self.check_readme()
            self.check_errors()
            self.check_strategy_flags()
            self.check_sorted_inputs()
            self.check_small_inputs()
            self.check_five_inputs()
            self.check_benchmark()
            self.check_strategy_comparison()
            self.check_provided_checker()
            self.check_valgrind()

        if mode in {"mandatory", "performance", "all"}:
            self.performance_runs(100, self.args.runs100, 500)
            self.performance_runs(500, self.args.runs500, 1000)

        if mode in {"bonus", "all"}:
            self.check_bonus(strict=(mode == "bonus"))

        if self.args.include_live:
            self.check_live_count_only()

        self.print_manual_checklist()
        self.print_summary()
        self.save_json()
        return 1 if any(result.status == "FAIL" for result in self.results) else 0

    def print_summary(self) -> None:
        counts = self.summary_counts()
        print("\nSummary: " + ", ".join(f"{key}={value}" for key, value in counts.items()))
        if counts["FAIL"]:
            print(self.color("Result: FAIL", "31"))
        else:
            print(self.color("Result: no automated failures", "32"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Test a 42 push_swap project against subject v1.1 and the review scale."
    )
    parser.add_argument("project", nargs="?", default=".", help="path to the push_swap repository (default: current directory)")
    parser.add_argument(
        "--mode",
        choices=("quick", "mandatory", "performance", "bonus", "all"),
        default="mandatory",
        help="quick skips 100/500 tests; mandatory includes them; all also tests bonus",
    )
    parser.add_argument("--runs100", type=int, default=3, help="number of 100-item random runs")
    parser.add_argument("--runs500", type=int, default=2, help="number of 500-item random runs")
    parser.add_argument("--seed", type=int, default=42, help="random seed")
    parser.add_argument("--timeout", type=float, default=5.0, help="normal command timeout in seconds")
    parser.add_argument("--performance-timeout", type=float, default=30.0, help="timeout for large/strategy tests")
    parser.add_argument("--build-timeout", type=float, default=60.0, help="timeout for make/norminette")
    parser.add_argument("--valgrind-timeout", type=float, default=30.0, help="timeout for each valgrind run")
    parser.add_argument("--skip-build", action="store_true", help="do not run make targets")
    parser.add_argument("--skip-norm", action="store_true", help="do not run norminette")
    parser.add_argument("--skip-valgrind", action="store_true", help="do not run valgrind")
    parser.add_argument("--exhaustive-five", action="store_true", help="also test all 120 five-number permutations")
    parser.add_argument("--include-live", action="store_true", help="test the review-only --count-only live-coding modification")
    parser.add_argument("--json-report", help="write all results to this JSON file")
    parser.add_argument("--verbose", action="store_true", help="show details for passing tests")
    parser.add_argument("--no-color", action="store_true", help="disable ANSI colors")
    args = parser.parse_args(argv)
    if args.runs100 < 1 or args.runs500 < 1:
        parser.error("--runs100 and --runs500 must be at least 1")
    return args


def main(argv: Sequence[str] | None = None) -> int:
    return Tester(parse_args(argv or sys.argv[1:])).run_all()


if __name__ == "__main__":
    raise SystemExit(main())
