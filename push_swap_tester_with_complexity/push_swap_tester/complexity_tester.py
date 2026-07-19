#!/usr/bin/env python3
"""Empirically examine push_swap operation-count growth.

This tool measures the number of generated push_swap operations for increasing input
sizes, validates every operation stream with an internal checker, normalizes the
measurements by the complexity claimed in push_swap subject v1.1, and writes CSV,
JSON, Markdown, and SVG reports.

Finite measurements cannot mathematically prove a Big-O bound. The verdicts produced
here are evidence labels only: SUPPORTED, INCONCLUSIVE, or CONTRADICTED.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import random
import re
import statistics
import subprocess
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Iterable, Sequence

ALLOWED_OPS = (
    "sa", "sb", "ss", "pa", "pb", "ra", "rb", "rr", "rra", "rrb", "rrr"
)
ALLOWED_OP_SET = set(ALLOWED_OPS)
STRATEGIES = ("simple", "medium", "complex", "adaptive")
BASES = {
    "simple": ("n^2", lambda n: float(n * n)),
    "medium": ("n*sqrt(n)", lambda n: float(n) * math.sqrt(n)),
    "complex": ("n*log2(n)", lambda n: float(n) * math.log2(max(n, 2))),
}
REGIME_EXPECTATION = {
    "low": "simple",
    "medium": "medium",
    "high": "complex",
}


@dataclass
class RunRecord:
    strategy: str
    scenario: str
    regime: str
    size: int
    run: int
    seed: int
    target_disorder: float | None
    actual_disorder: float
    operation_count: int | None
    duration_seconds: float
    valid: bool
    timed_out: bool
    returncode: int | None
    bench_strategy: str | None
    bench_complexity: str | None
    error: str


@dataclass
class SummaryRecord:
    strategy: str
    scenario: str
    regime: str
    expected_basis: str
    size: int
    runs: int
    valid_runs: int
    min_operations: float | None
    median_operations: float | None
    mean_operations: float | None
    max_operations: float | None
    stdev_operations: float | None
    normalized_median: float | None
    normalized_max: float | None
    median_disorder: float | None
    median_duration_seconds: float | None


@dataclass
class AnalysisRecord:
    strategy: str
    scenario: str
    regime: str
    expected_basis: str
    sizes_used: list[int]
    empirical_log_slope: float | None
    normalized_last_first: float | None
    normalized_tail_spread: float | None
    median_doubling_ratio: float | None
    basis_fit_r2: float | None
    correctness_failures: int
    bench_mismatches: int
    verdict: str
    explanation: str


def parse_int_list(text: str) -> list[int]:
    values: list[int] = []
    for item in text.split(","):
        item = item.strip()
        if not item:
            continue
        try:
            value = int(item)
        except ValueError as exc:
            raise argparse.ArgumentTypeError(f"invalid integer in list: {item!r}") from exc
        values.append(value)
    if not values:
        raise argparse.ArgumentTypeError("list must contain at least one integer")
    if any(value < 2 for value in values):
        raise argparse.ArgumentTypeError("all sizes must be at least 2")
    if sorted(set(values)) != values:
        raise argparse.ArgumentTypeError("sizes must be unique and strictly increasing")
    return values


def parse_name_list(text: str, allowed: set[str], label: str) -> list[str]:
    values = [item.strip() for item in text.split(",") if item.strip()]
    unknown = [item for item in values if item not in allowed]
    if unknown:
        raise argparse.ArgumentTypeError(
            f"unknown {label}: {', '.join(unknown)}; allowed: {', '.join(sorted(allowed))}"
        )
    if not values:
        raise argparse.ArgumentTypeError(f"at least one {label} is required")
    return values


def inversion_count(values: Sequence[int]) -> int:
    """Count inversions in O(n log n) time without modifying the input."""
    source = list(values)
    scratch = [0] * len(source)

    def count_range(begin: int, end: int) -> int:
        if end - begin <= 1:
            return 0
        middle = (begin + end) // 2
        total = count_range(begin, middle) + count_range(middle, end)
        left, right, write = begin, middle, begin
        while left < middle and right < end:
            if source[left] <= source[right]:
                scratch[write] = source[left]
                left += 1
            else:
                scratch[write] = source[right]
                total += middle - left
                right += 1
            write += 1
        while left < middle:
            scratch[write] = source[left]
            left += 1
            write += 1
        while right < end:
            scratch[write] = source[right]
            right += 1
            write += 1
        source[begin:end] = scratch[begin:end]
        return total

    return count_range(0, len(source))


def disorder(values: Sequence[int]) -> float:
    pairs = len(values) * (len(values) - 1) // 2
    return inversion_count(values) / pairs if pairs else 0.0


def random_lehmer_code(n: int, inversions: int, rng: random.Random) -> list[int]:
    """Create a randomized Lehmer code with an exact inversion count."""
    max_inversions = n * (n - 1) // 2
    if inversions < 0 or inversions > max_inversions:
        raise ValueError("inversion count outside valid range")
    code: list[int] = []
    remaining = inversions
    future_capacity = max_inversions
    for index in range(n):
        cap = n - 1 - index
        future_capacity -= cap
        minimum = max(0, remaining - future_capacity)
        maximum = min(cap, remaining)
        digit = rng.randint(minimum, maximum) if maximum > minimum else minimum
        code.append(digit)
        remaining -= digit
    if remaining != 0:
        raise AssertionError("failed to distribute inversions")
    return code


def permutation_with_disorder(n: int, target: float, rng: random.Random) -> list[int]:
    """Return a permutation whose inversion ratio is as close as rounding permits."""
    if not 0.0 <= target <= 1.0:
        raise ValueError("target disorder must be between 0 and 1")
    pairs = n * (n - 1) // 2
    target_inversions = round(target * pairs)
    code = random_lehmer_code(n, target_inversions, rng)
    remaining = list(range(n))
    permutation: list[int] = []
    for digit in code:
        permutation.append(remaining.pop(digit))
    return permutation


def generate_input(n: int, scenario: str, rng: random.Random) -> tuple[list[int], float | None, str]:
    if scenario == "random":
        values = list(range(n))
        rng.shuffle(values)
        return values, None, "random"
    if scenario == "reverse":
        return list(range(n - 1, -1, -1)), 1.0, "high"
    if scenario == "low":
        target = 0.10
    elif scenario == "medium":
        target = 0.35
    elif scenario == "high":
        target = 0.75
    else:
        raise ValueError(f"unknown scenario: {scenario}")
    return permutation_with_disorder(n, target, rng), target, scenario


def parse_operations(stdout: str) -> tuple[list[str] | None, str]:
    if stdout == "":
        return [], ""
    if "\r" in stdout:
        return None, "stdout contains carriage returns"
    if not stdout.endswith("\n"):
        return None, "operation stream does not end with a newline"
    operations = stdout.splitlines()
    invalid = [op for op in operations if op not in ALLOWED_OP_SET]
    if invalid:
        return None, "invalid operation(s): " + ", ".join(repr(op) for op in invalid[:5])
    return operations, ""


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
    return a, b


def normalize_complexity_text(text: str) -> str | None:
    compact = text.lower().replace(" ", "").replace("√", "sqrt")
    compact = compact.replace("^", "").replace("²", "2")
    flattened = compact.replace("(", "").replace(")", "").replace("*", "")
    if "nlog2n" in flattened or "nlogn" in flattened:
        return "n*log2(n)"
    if "nsqrtn" in flattened:
        return "n*sqrt(n)"
    if "on2" in flattened or re.search(r"\bn2\b", flattened):
        return "n^2"
    return None


def parse_bench(stderr: str) -> tuple[str | None, str | None]:
    lower = stderr.lower()
    strategy: str | None = None
    for name in ("simple", "medium", "complex", "adaptive"):
        if re.search(rf"\b{name}\b", lower):
            strategy = name
            break
    complexity = normalize_complexity_text(stderr)
    return strategy, complexity


def basis_for(strategy: str, regime: str) -> tuple[str, callable]:
    effective = strategy
    if strategy == "adaptive":
        effective = REGIME_EXPECTATION.get(regime, "complex")
    return BASES[effective]


def linear_regression(xs: Sequence[float], ys: Sequence[float]) -> tuple[float, float, float] | None:
    if len(xs) != len(ys) or len(xs) < 2:
        return None
    x_mean = statistics.fmean(xs)
    y_mean = statistics.fmean(ys)
    denominator = sum((x - x_mean) ** 2 for x in xs)
    if denominator == 0:
        return None
    slope = sum((x - x_mean) * (y - y_mean) for x, y in zip(xs, ys)) / denominator
    intercept = y_mean - slope * x_mean
    predicted = [intercept + slope * x for x in xs]
    total = sum((y - y_mean) ** 2 for y in ys)
    residual = sum((y - p) ** 2 for y, p in zip(ys, predicted))
    r2 = 1.0 - residual / total if total > 0 else 1.0
    return slope, intercept, r2


def through_origin_r2(xs: Sequence[float], ys: Sequence[float]) -> float | None:
    if len(xs) != len(ys) or len(xs) < 2:
        return None
    denominator = sum(x * x for x in xs)
    if denominator == 0:
        return None
    coefficient = sum(x * y for x, y in zip(xs, ys)) / denominator
    predicted = [coefficient * x for x in xs]
    mean_y = statistics.fmean(ys)
    total = sum((y - mean_y) ** 2 for y in ys)
    residual = sum((y - p) ** 2 for y, p in zip(ys, predicted))
    return 1.0 - residual / total if total > 0 else 1.0


def expected_effective_slope(strategy: str, regime: str, sizes: Sequence[int]) -> float:
    effective = strategy if strategy != "adaptive" else REGIME_EXPECTATION.get(regime, "complex")
    if effective == "simple":
        return 2.0
    if effective == "medium":
        return 1.5
    geometric_n = math.exp(statistics.fmean(math.log(n) for n in sizes))
    return 1.0 + 1.0 / math.log(max(geometric_n, math.e + 0.001))


def format_number(value: float | int | None, digits: int = 3) -> str:
    if value is None:
        return "—"
    if isinstance(value, int):
        return str(value)
    return f"{value:.{digits}f}"


def xml_escape(text: str) -> str:
    return (
        text.replace("&", "&amp;")
        .replace("<", "&lt;")
        .replace(">", "&gt;")
        .replace('"', "&quot;")
    )


class ComplexityTester:
    def __init__(self, args: argparse.Namespace) -> None:
        self.args = args
        self.project = Path(args.project).expanduser().resolve()
        self.push_swap = self.project / "push_swap"
        self.out_dir = Path(args.out_dir).expanduser().resolve()
        self.records: list[RunRecord] = []
        self.summaries: list[SummaryRecord] = []
        self.analyses: list[AnalysisRecord] = []

    def build(self) -> bool:
        if self.args.skip_build:
            return self.push_swap.is_file() and os.access(self.push_swap, os.X_OK)
        try:
            result = subprocess.run(
                ["make"],
                cwd=self.project,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=self.args.build_timeout,
                check=False,
                env={**os.environ, "LC_ALL": "C", "LANG": "C"},
            )
        except (OSError, subprocess.TimeoutExpired) as exc:
            print(f"build failed: {exc}", file=sys.stderr)
            return False
        if result.returncode != 0:
            print(result.stdout, end="", file=sys.stderr)
            print(result.stderr, end="", file=sys.stderr)
            return False
        return self.push_swap.is_file() and os.access(self.push_swap, os.X_OK)

    def scenarios_for(self, strategy: str) -> list[str]:
        if strategy == "adaptive":
            return self.args.adaptive_scenarios
        return self.args.scenarios

    def run_once(
        self,
        strategy: str,
        scenario: str,
        size: int,
        run_index: int,
        seed: int,
    ) -> RunRecord:
        rng = random.Random(seed)
        values, target_disorder, regime = generate_input(size, scenario, rng)
        measured_disorder = disorder(values)
        command = [str(self.push_swap)]
        if self.args.bench:
            command.append("--bench")
        command.append(f"--{strategy}")
        command.extend(str(value) for value in values)
        started = time.monotonic()
        try:
            result = subprocess.run(
                command,
                cwd=self.project,
                text=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                timeout=self.args.timeout,
                check=False,
                env={**os.environ, "LC_ALL": "C", "LANG": "C"},
            )
            elapsed = time.monotonic() - started
        except subprocess.TimeoutExpired as exc:
            stdout = exc.stdout.decode(errors="replace") if isinstance(exc.stdout, bytes) else (exc.stdout or "")
            stderr = exc.stderr.decode(errors="replace") if isinstance(exc.stderr, bytes) else (exc.stderr or "")
            return RunRecord(
                strategy, scenario, regime, size, run_index, seed, target_disorder,
                measured_disorder, None, time.monotonic() - started, False, True,
                None, None, None, f"timeout after {self.args.timeout}s; stdout={stdout[:120]!r}; stderr={stderr[:120]!r}",
            )
        except OSError as exc:
            return RunRecord(
                strategy, scenario, regime, size, run_index, seed, target_disorder,
                measured_disorder, None, time.monotonic() - started, False, False,
                127, None, None, str(exc),
            )

        operations, parse_error = parse_operations(result.stdout)
        bench_strategy, bench_complexity = parse_bench(result.stderr) if self.args.bench else (None, None)
        if result.returncode != 0:
            error = f"return code {result.returncode}; stderr={result.stderr[:240]!r}"
            valid = False
            operation_count = len(operations) if operations is not None else None
        elif operations is None:
            error = parse_error
            valid = False
            operation_count = None
        else:
            final_a, final_b = execute_operations(values, operations)
            valid = final_a == sorted(values) and not final_b
            operation_count = len(operations)
            error = "" if valid else f"invalid final stacks: a[:10]={final_a[:10]}, b[:10]={final_b[:10]}"
        return RunRecord(
            strategy, scenario, regime, size, run_index, seed, target_disorder,
            measured_disorder, operation_count, elapsed, valid, False,
            result.returncode, bench_strategy, bench_complexity, error,
        )

    def execute(self) -> None:
        total = sum(
            len(self.scenarios_for(strategy)) * len(self.args.sizes) * self.args.runs
            for strategy in self.args.strategies
        )
        current = 0
        for strategy_index, strategy in enumerate(self.args.strategies):
            for scenario_index, scenario in enumerate(self.scenarios_for(strategy)):
                for size_index, size in enumerate(self.args.sizes):
                    for run_index in range(1, self.args.runs + 1):
                        current += 1
                        seed = (
                            self.args.seed
                            + strategy_index * 10_000_000
                            + scenario_index * 1_000_000
                            + size_index * 10_000
                            + run_index
                        )
                        print(
                            f"[{current:>3}/{total}] --{strategy:<8} {scenario:<7} "
                            f"n={size:<5} run={run_index}",
                            flush=True,
                        )
                        record = self.run_once(strategy, scenario, size, run_index, seed)
                        self.records.append(record)
                        if not record.valid:
                            print(f"           FAIL: {record.error}", file=sys.stderr)

    def summarize(self) -> None:
        groups: dict[tuple[str, str, str, int], list[RunRecord]] = {}
        for record in self.records:
            groups.setdefault(
                (record.strategy, record.scenario, record.regime, record.size), []
            ).append(record)
        for (strategy, scenario, regime, size), records in sorted(groups.items()):
            valid_counts = [
                record.operation_count
                for record in records
                if record.valid and record.operation_count is not None
            ]
            valid_disorders = [record.actual_disorder for record in records if record.valid]
            valid_durations = [record.duration_seconds for record in records if record.valid]
            basis_name, basis_function = basis_for(strategy, regime)
            median_ops = statistics.median(valid_counts) if valid_counts else None
            stdev = statistics.stdev(valid_counts) if len(valid_counts) >= 2 else (0.0 if valid_counts else None)
            self.summaries.append(
                SummaryRecord(
                    strategy=strategy,
                    scenario=scenario,
                    regime=regime,
                    expected_basis=basis_name,
                    size=size,
                    runs=len(records),
                    valid_runs=len(valid_counts),
                    min_operations=min(valid_counts) if valid_counts else None,
                    median_operations=median_ops,
                    mean_operations=statistics.fmean(valid_counts) if valid_counts else None,
                    max_operations=max(valid_counts) if valid_counts else None,
                    stdev_operations=stdev,
                    normalized_median=(median_ops / basis_function(size)) if median_ops is not None else None,
                    normalized_max=(max(valid_counts) / basis_function(size)) if valid_counts else None,
                    median_disorder=statistics.median(valid_disorders) if valid_disorders else None,
                    median_duration_seconds=statistics.median(valid_durations) if valid_durations else None,
                )
            )

    def analyze(self) -> None:
        groups: dict[tuple[str, str, str], list[SummaryRecord]] = {}
        for summary in self.summaries:
            groups.setdefault((summary.strategy, summary.scenario, summary.regime), []).append(summary)
        for key, summaries in sorted(groups.items()):
            strategy, scenario, regime = key
            summaries.sort(key=lambda item: item.size)
            usable = [
                item for item in summaries
                if item.valid_runs == item.runs
                and item.median_operations is not None
                and item.median_operations > 0
                and item.normalized_median is not None
                and item.max_operations is not None
                and item.normalized_max is not None
            ]
            group_records = [
                record for record in self.records
                if (record.strategy, record.scenario, record.regime) == key
            ]
            correctness_failures = sum(not record.valid for record in group_records)
            expected_basis, basis_function = basis_for(strategy, regime)
            expected_strategy = strategy if strategy != "adaptive" else REGIME_EXPECTATION.get(regime)
            bench_mismatches = 0
            if self.args.bench:
                for record in group_records:
                    if not record.valid:
                        continue
                    if record.bench_complexity and record.bench_complexity != expected_basis:
                        bench_mismatches += 1
                    if strategy == "adaptive" and record.bench_strategy:
                        if record.bench_strategy not in {"adaptive", expected_strategy}:
                            bench_mismatches += 1

            def selected_operations(item: SummaryRecord) -> float:
                if self.args.analysis_stat == "max":
                    return float(item.max_operations)
                return float(item.median_operations)

            def selected_normalized(item: SummaryRecord) -> float:
                if self.args.analysis_stat == "max":
                    return float(item.normalized_max)
                return float(item.normalized_median)

            slope = None
            normalized_last_first = None
            normalized_tail_spread = None
            doubling_ratio = None
            fit_r2 = None
            sizes_used = [item.size for item in usable]
            if len(usable) >= 2:
                log_fit = linear_regression(
                    [math.log(item.size) for item in usable],
                    [math.log(selected_operations(item)) for item in usable],
                )
                slope = log_fit[0] if log_fit else None
                first_norm = selected_normalized(usable[0])
                last_norm = selected_normalized(usable[-1])
                normalized_last_first = last_norm / first_norm if first_norm > 0 else None
                tail = usable[max(0, len(usable) // 2) :]
                tail_norms = [selected_normalized(item) for item in tail]
                normalized_tail_spread = max(tail_norms) / min(tail_norms) if min(tail_norms) > 0 else None
                growth: list[float] = []
                for before, after in zip(usable, usable[1:]):
                    if selected_operations(before) > 0 and after.size / before.size >= 1.8:
                        growth.append(selected_operations(after) / selected_operations(before))
                doubling_ratio = statistics.median(growth) if growth else None
                fit_r2 = through_origin_r2(
                    [basis_function(item.size) for item in usable],
                    [selected_operations(item) for item in usable],
                )

            if correctness_failures:
                verdict = "CONTRADICTED"
                explanation = f"{correctness_failures} run(s) failed correctness; complexity evidence is invalid until fixed."
            elif len(usable) < 4:
                verdict = "INCONCLUSIVE"
                explanation = "At least four valid non-zero input sizes are needed for a useful scaling comparison."
            else:
                expected_slope = expected_effective_slope(strategy, regime, sizes_used)
                severe_ratio = normalized_last_first is not None and normalized_last_first > 2.50
                severe_tail = normalized_tail_spread is not None and normalized_tail_spread > 2.50
                severe_slope = slope is not None and slope > expected_slope + 0.45
                stable_ratio = normalized_last_first is not None and normalized_last_first <= 1.75
                stable_tail = normalized_tail_spread is not None and normalized_tail_spread <= 2.00
                acceptable_slope = slope is not None and slope <= expected_slope + 0.35
                if (severe_ratio and severe_slope) or (severe_tail and severe_slope):
                    verdict = "CONTRADICTED"
                    explanation = (
                        "Operation growth rises materially faster than the claimed basis over the tested range. "
                        "Inspect larger sizes and the algorithm's worst-case argument."
                    )
                elif stable_ratio and stable_tail and acceptable_slope:
                    verdict = "SUPPORTED"
                    explanation = (
                        "The normalized operation count remains reasonably bounded and the empirical slope "
                        "does not exceed the claimed growth by the configured tolerance."
                    )
                else:
                    verdict = "INCONCLUSIVE"
                    explanation = (
                        "The measurements neither clearly support nor clearly contradict the claim. "
                        "Increase sizes/runs and include a justified worst-case family."
                    )
            if bench_mismatches:
                explanation += f" Bench metadata had {bench_mismatches} mismatch(es)."
            self.analyses.append(
                AnalysisRecord(
                    strategy=strategy,
                    scenario=scenario,
                    regime=regime,
                    expected_basis=expected_basis,
                    sizes_used=sizes_used,
                    empirical_log_slope=slope,
                    normalized_last_first=normalized_last_first,
                    normalized_tail_spread=normalized_tail_spread,
                    median_doubling_ratio=doubling_ratio,
                    basis_fit_r2=fit_r2,
                    correctness_failures=correctness_failures,
                    bench_mismatches=bench_mismatches,
                    verdict=verdict,
                    explanation=explanation,
                )
            )

    def write_csv(self) -> None:
        self.out_dir.mkdir(parents=True, exist_ok=True)
        samples_path = self.out_dir / "complexity_samples.csv"
        with samples_path.open("w", newline="", encoding="utf-8") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(asdict(self.records[0]).keys()))
            writer.writeheader()
            for record in self.records:
                writer.writerow(asdict(record))
        summary_path = self.out_dir / "complexity_summary.csv"
        with summary_path.open("w", newline="", encoding="utf-8") as handle:
            writer = csv.DictWriter(handle, fieldnames=list(asdict(self.summaries[0]).keys()))
            writer.writeheader()
            for summary in self.summaries:
                writer.writerow(asdict(summary))

    def write_json(self) -> None:
        payload = {
            "metadata": {
                "project": str(self.project),
                "push_swap": str(self.push_swap),
                "seed": self.args.seed,
                "sizes": self.args.sizes,
                "runs": self.args.runs,
                "strategies": self.args.strategies,
                "scenarios": self.args.scenarios,
                "adaptive_scenarios": self.args.adaptive_scenarios,
                "bench": self.args.bench,
                "warning": "Finite empirical tests do not prove an asymptotic Big-O bound.",
            },
            "analysis": [asdict(item) for item in self.analyses],
            "summary": [asdict(item) for item in self.summaries],
            "samples": [asdict(item) for item in self.records],
        }
        (self.out_dir / "complexity_report.json").write_text(
            json.dumps(payload, ensure_ascii=False, indent=2) + "\n",
            encoding="utf-8",
        )

    def write_svg(self) -> list[str]:
        chart_names: list[str] = []
        groups: dict[tuple[str, str, str], list[SummaryRecord]] = {}
        for summary in self.summaries:
            groups.setdefault((summary.strategy, summary.scenario, summary.regime), []).append(summary)
        palette = ["#2563eb", "#dc2626", "#059669", "#7c3aed", "#d97706", "#0891b2"]
        width, height = 900, 560
        left, right, top, bottom = 90, 30, 45, 75
        for index, (key, series) in enumerate(sorted(groups.items())):
            strategy, scenario, regime = key
            valid = [item for item in sorted(series, key=lambda x: x.size) if item.median_operations and item.median_operations > 0 and item.max_operations]
            if len(valid) < 2:
                continue
            x_values = [math.log10(item.size) for item in valid]
            chart_values = [float(item.max_operations) if self.args.analysis_stat == "max" else float(item.median_operations) for item in valid]
            y_values = [math.log10(value) for value in chart_values]
            x_min, x_max = min(x_values), max(x_values)
            y_min, y_max = min(y_values), max(y_values)
            if x_min == x_max:
                x_max += 1
            if y_min == y_max:
                y_max += 1
            x_pad = (x_max - x_min) * 0.08
            y_pad = (y_max - y_min) * 0.12
            x_min -= x_pad
            x_max += x_pad
            y_min -= y_pad
            y_max += y_pad

            def sx(value: float) -> float:
                return left + (value - x_min) / (x_max - x_min) * (width - left - right)

            def sy(value: float) -> float:
                return top + (y_max - value) / (y_max - y_min) * (height - top - bottom)

            color = palette[index % len(palette)]
            points = " ".join(f"{sx(x):.1f},{sy(y):.1f}" for x, y in zip(x_values, y_values))
            title = f"--{strategy} / {scenario}: {self.args.analysis_stat} operation count"
            lines = [
                f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}">',
                '<rect width="100%" height="100%" fill="white"/>',
                f'<text x="{width/2}" y="28" text-anchor="middle" font-family="sans-serif" font-size="20">{xml_escape(title)}</text>',
                f'<line x1="{left}" y1="{height-bottom}" x2="{width-right}" y2="{height-bottom}" stroke="#111"/>',
                f'<line x1="{left}" y1="{top}" x2="{left}" y2="{height-bottom}" stroke="#111"/>',
            ]
            for item, value, x, y in zip(valid, chart_values, x_values, y_values):
                px, py = sx(x), sy(y)
                lines.extend([
                    f'<circle cx="{px:.1f}" cy="{py:.1f}" r="5" fill="{color}"/>',
                    f'<text x="{px:.1f}" y="{height-bottom+22}" text-anchor="middle" font-family="sans-serif" font-size="12">{item.size}</text>',
                    f'<text x="{px+7:.1f}" y="{py-8:.1f}" font-family="sans-serif" font-size="11">{int(value)}</text>',
                ])
            lines.append(f'<polyline points="{points}" fill="none" stroke="{color}" stroke-width="3"/>')
            lines.extend([
                f'<text x="{width/2}" y="{height-22}" text-anchor="middle" font-family="sans-serif" font-size="14">input size n (log scale)</text>',
                f'<text x="22" y="{height/2}" transform="rotate(-90 22 {height/2})" text-anchor="middle" font-family="sans-serif" font-size="14">{xml_escape(self.args.analysis_stat)} operations (log scale)</text>',
                '</svg>',
            ])
            filename = f"chart_{strategy}_{scenario}.svg"
            (self.out_dir / filename).write_text("\n".join(lines) + "\n", encoding="utf-8")
            chart_names.append(filename)
        return chart_names

    def write_markdown(self, chart_names: Sequence[str]) -> None:
        lines = [
            "# push_swap operation-count complexity report",
            "",
            "> This is empirical evidence, not a mathematical proof of Big-O complexity. ",
            "> A finite test can expose growth that is too fast, but it cannot establish an upper bound for every input size and every valid input.",
            "",
            "## Configuration",
            "",
            f"- Project: `{self.project}`",
            f"- Sizes: `{', '.join(map(str, self.args.sizes))}`",
            f"- Runs per size: `{self.args.runs}`",
            f"- Strategies: `{', '.join(self.args.strategies)}`",
            f"- Forced-strategy scenarios: `{', '.join(self.args.scenarios)}`",
            f"- Adaptive scenarios: `{', '.join(self.args.adaptive_scenarios)}`",
            f"- Seed: `{self.args.seed}`",
            f"- Scaling statistic: `{self.args.analysis_stat}` operations observed per size",
            "",
            "## Verdicts",
            "",
            "| Strategy | Scenario | Claimed basis | Slope | normalized last/first | tail spread | R² | Correctness failures | Bench mismatches | Verdict |",
            "|---|---:|---:|---:|---:|---:|---:|---:|---:|---|",
        ]
        for item in self.analyses:
            lines.append(
                "| "
                + " | ".join([
                    f"`--{item.strategy}`",
                    item.scenario,
                    f"`{item.expected_basis}`",
                    format_number(item.empirical_log_slope),
                    format_number(item.normalized_last_first),
                    format_number(item.normalized_tail_spread),
                    format_number(item.basis_fit_r2),
                    str(item.correctness_failures),
                    str(item.bench_mismatches),
                    f"**{item.verdict}**",
                ])
                + " |"
            )
        lines.extend(["", "## Interpretation", ""])
        for item in self.analyses:
            lines.append(
                f"- `--{item.strategy}` / **{item.scenario}** / `{item.expected_basis}`: "
                f"**{item.verdict}** — {item.explanation}"
            )
        lines.extend([
            "",
            "## Median operation counts",
            "",
            "| Strategy | Scenario | n | Disorder | Median | Mean | Min | Max | Stddev | Median / basis | Max / basis |",
            "|---|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|",
        ])
        for item in self.summaries:
            lines.append(
                "| "
                + " | ".join([
                    f"`--{item.strategy}`",
                    item.scenario,
                    str(item.size),
                    format_number(item.median_disorder),
                    format_number(item.median_operations, 1),
                    format_number(item.mean_operations, 1),
                    format_number(item.min_operations, 0),
                    format_number(item.max_operations, 0),
                    format_number(item.stdev_operations, 1),
                    format_number(item.normalized_median, 5),
                    format_number(item.normalized_max, 5),
                ])
                + " |"
            )
        if chart_names:
            lines.extend(["", "## Charts", ""])
            for filename in chart_names:
                lines.append(f"![{filename}]({filename})")
                lines.append("")
        lines.extend([
            "## What the metrics mean",
            "",
            "- **Slope**: fitted exponent in `operations ≈ C · n^p` on a log-log plot. It is descriptive, not a proof.",
            f"- **normalized last/first**: `({self.args.analysis_stat}_ops / claimed_basis)` at the largest size divided by the same value at the smallest size. A bounded ratio supports the claimed upper-bound shape.",
            "- **tail spread**: maximum divided by minimum normalized value in the larger half of tested sizes.",
            "- **R²**: fit quality for `operations ≈ C · claimed_basis(n)` through the origin. High R² alone is not sufficient evidence.",
            "",
            "## Stronger defense evidence",
            "",
            "Use this report together with a code-level upper-bound argument. Identify the loops or passes that generate operations and show why their total is bounded by `n²`, `n√n`, or `n log n`. For adaptive mode, separately justify the disorder thresholds and the method selected in each regime.",
            "",
        ])
        (self.out_dir / "complexity_report.md").write_text("\n".join(lines), encoding="utf-8")

    def print_summary(self) -> None:
        print("\nComplexity evidence summary")
        print("=" * 88)
        for item in self.analyses:
            print(
                f"--{item.strategy:<8} {item.scenario:<7} {item.expected_basis:<12} "
                f"slope={format_number(item.empirical_log_slope):>6} "
                f"norm={format_number(item.normalized_last_first):>6} "
                f"R2={format_number(item.basis_fit_r2):>6}  {item.verdict}"
            )
        print("=" * 88)
        print(f"Report directory: {self.out_dir}")
        print("Reminder: finite measurements can support or contradict a claim; they cannot prove Big-O.")

    def run(self) -> int:
        if not self.project.is_dir():
            print(f"not a directory: {self.project}", file=sys.stderr)
            return 2
        if not self.build():
            print(f"could not build/find executable: {self.push_swap}", file=sys.stderr)
            return 2
        self.execute()
        self.summarize()
        self.analyze()
        self.write_csv()
        self.write_json()
        charts = self.write_svg()
        self.write_markdown(charts)
        self.print_summary()
        bad_correctness = any(not record.valid for record in self.records)
        contradicted = any(item.verdict == "CONTRADICTED" for item in self.analyses)
        if bad_correctness:
            return 1
        if self.args.strict and contradicted:
            return 1
        return 0


def self_test() -> int:
    for n in range(2, 18):
        pairs = n * (n - 1) // 2
        for target_inversions in (0, 1, pairs // 4, pairs // 2, max(0, pairs - 1), pairs):
            rng = random.Random(n * 1000 + target_inversions)
            code = random_lehmer_code(n, target_inversions, rng)
            remaining = list(range(n))
            permutation = [remaining.pop(digit) for digit in code]
            actual = round(disorder(permutation) * pairs)
            if actual != target_inversions:
                print(
                    f"self-test failed: n={n} target={target_inversions} actual={actual}",
                    file=sys.stderr,
                )
                return 1
    values = [2, 1, 3]
    operations = ["sa"]
    final_a, final_b = execute_operations(values, operations)
    if final_a != [1, 2, 3] or final_b:
        print("self-test failed: operation simulator", file=sys.stderr)
        return 1
    if normalize_complexity_text("O(n log n)") != "n*log2(n)":
        print("self-test failed: complexity parser", file=sys.stderr)
        return 1
    print("self-test: PASS")
    return 0


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Measure push_swap operation-count scaling for simple O(n²), medium O(n√n), "
            "complex O(n log n), and adaptive disorder regimes."
        )
    )
    parser.add_argument("project", nargs="?", default=".", help="path to push_swap repository")
    parser.add_argument(
        "--sizes",
        type=parse_int_list,
        default=[16, 32, 64, 128, 256],
        help="strictly increasing comma-separated sizes (default: 16,32,64,128,256)",
    )
    parser.add_argument("--runs", type=int, default=7, help="runs per strategy/scenario/size (default: 7)")
    parser.add_argument(
        "--strategies",
        type=lambda text: parse_name_list(text, set(STRATEGIES), "strategy"),
        default=list(STRATEGIES),
        help="comma-separated strategies (default: simple,medium,complex,adaptive)",
    )
    parser.add_argument(
        "--scenarios",
        type=lambda text: parse_name_list(text, {"random", "reverse", "low", "medium", "high"}, "scenario"),
        default=["random", "reverse"],
        help="forced-strategy scenarios (default: random,reverse)",
    )
    parser.add_argument(
        "--adaptive-scenarios",
        type=lambda text: parse_name_list(text, {"low", "medium", "high"}, "adaptive scenario"),
        default=["low", "medium", "high"],
        help="adaptive disorder regimes (default: low,medium,high)",
    )
    parser.add_argument("--seed", type=int, default=42, help="base random seed")
    parser.add_argument("--timeout", type=float, default=60.0, help="timeout per execution in seconds")
    parser.add_argument("--build-timeout", type=float, default=120.0, help="timeout for make")
    parser.add_argument("--skip-build", action="store_true", help="do not run make")
    parser.add_argument("--no-bench", dest="bench", action="store_false", help="do not pass --bench or inspect metadata")
    parser.set_defaults(bench=True)
    parser.add_argument("--out-dir", default="complexity_report", help="report output directory")
    parser.add_argument("--analysis-stat", choices=("max", "median"), default="max", help="statistic used for scaling verdicts (default: max)")
    parser.add_argument("--strict", action="store_true", help="exit non-zero on heuristic CONTRADICTED verdict")
    parser.add_argument("--self-test", action="store_true", help="test this script itself and exit")
    args = parser.parse_args(argv)
    if args.runs < 1:
        parser.error("--runs must be at least 1")
    if args.timeout <= 0 or args.build_timeout <= 0:
        parser.error("timeouts must be positive")
    return args


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    if args.self_test:
        return self_test()
    return ComplexityTester(args).run()


if __name__ == "__main__":
    raise SystemExit(main())
