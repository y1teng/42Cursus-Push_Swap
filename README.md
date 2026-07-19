*This project has been created as part of the 42 curriculum by ykomori, ayaito*

# push_swap

-----

## Description

限られた2種類のスタック(`a`, `b`)と限られた命令セット(`sa`/`sb`/`ss`, `pa`/`pb`, `ra`/`rb`/`rr`, `rra`/`rrb`/`rrr`)だけを使い、重複のない整数列をできるだけ少ない手数で昇順に並び替えるプログラム。
入力サイズや乱雑さ(disorder)に応じて複数のソート戦略(`Simple`/`Medium`/`Complex`/`Adaptive`)を切り替えられる。

-----

## Instructions

### Requirements

- Compiler: `cc`
- Flags: `-Wall -Wextra -Werror`
- C standard: C99 (no `-std=c99` flag)

### Compilation

| コマンド | 説明 |
|---|---|
| `make` / `make all` | ソースをコンパイルし `push_swap` を生成する |
| `make clean` | `.o` ファイルと `obj/` ディレクトリを削除する |
| `make fclean` | `clean` に加えて `push_swap` 本体も削除する |
| `make re` | `fclean` してから `all` を実行する(フルリビルド) |

```sh
make
```

### 実行

```sh
./push_swap 2 1 3 4
./push_swap "2 1 3 4"          # 空白区切りの1引数でも可
```

### 戦略フラグ

| フラグ | 戦略 | 複雑性クラス |
|---|---|---|
| (指定なし) / `--adaptive` | 乱雑さ(disorder)に応じて自動選択(デフォルト) | 下記参照 |
| `--simple` | 選択ソート | O(n²) |
| `--medium` | チャンク分割ソート | O(n√n) |
| `--complex` | 基数ソート(LSD radix) | O(n log n) |
| `--bench` | 他フラグと併用可。ソート後にstderrへ統計情報を出力 | - |

### テスト例

```sh
# チェッカーで正当性確認(checker_linuxをchmodしておく)
chmod 755 checker_linux
ARG="2 1 0"; ./push_swap $ARG | ./checker_linux $ARG

# ランダム入力(shuf使用、zshではsetopt shwordsplitか ${=args} で単語分割)
args=$(shuf -i 1-1000000 -n 100)
./push_swap ${=args} | ./checker_linux ${=args}

# 手数の確認
./push_swap ${=args} | wc -l

# --benchモード(stdoutは操作列、stderrに統計)
./push_swap --bench ${=args} 2>&1 1>/dev/null
```

### NOTE

このシェルは **zsh** を想定している。zshは `$args` を自動で単語分割しないため、`setopt shwordsplit` するか `${=args}` を使うこと(bashならデフォルトで分割される)。

-----

## Detailed explanation and justification of the algorithms

### Simple — O(n²)(選択ソート)

各反復で残っている要素中の最小値を探し、最短方向へ回転してトップへ寄せてから `b` へ push する。反復 `k`(k=1..n)時点で `a` のサイズは `n-k+1` で、回転コストはその位置に比例するため:

```
総操作数 ≈ Σ(k=1 to n) O(n-k+1) = O(n²)
```

要素数が極めて少ない場合や、すでにほぼソート済みの入力(`disorder < 0.2`)で有効。

### Medium — O(n√n)(チャンク分割ソート)

値の範囲を `c = ⌈√n⌉` 個のチャンク(バケット)に分割し、チャンクごとに `a` から該当する要素を探して `b` へ移送、その後 `b` から `a` へ挿入し直す。チャンク数とチャンクサイズのトレードオフ:

```
分配フェーズ: O(c × n) = O(√n × n)
再挿入フェーズ: O(c × (n/c)²) = O(n²/c) = O(n^1.5)
c = √n のとき両者が釣り合い、合計 O(n√n)
```

### Complex — O(n log n)(LSD基数ソート)

比較を使わず、値を「順位」(0〜n-1の整数)に変換したのち、下位ビットから順に `a`/`b` へ振り分けるビット分配ソート。ビット数は `bits = ⌈log2(n)⌉` で決まり、各ビットパスはちょうど n 回の push/rotate 操作 + 最大 n 回の push-back で構成される:

```
1パスのコスト = O(n)
パス数 = ⌈log2(n)⌉
総操作数 = O(n log n)
```

比較ソートの `Ω(n log n)` 下限とは異なる仕組み(基数ソート)のため、この計算量になる。

### Adaptive — 上記3つ + 閾値による自動選択

事前に `disorder`(0.0〜1.0の乱雑度)を計算し、以下の閾値で戦略を決定する:

```
disorder < 0.2        → Simple  (O(n²))
0.2 ≤ disorder < 0.5   → Medium  (O(n√n))
disorder ≥ 0.5         → Complex (O(n log n))
```

サイズが2, 3, 5の入力は上記の閾値によらず、専用の小規模ルーチン(直接swap / `sort_three` / `sort_five`)を使うため常に `Simple` 相当として扱う。

### 実測による裏付け

`--bench` の手数と、n を変えながら測定した回帰分析(傾き・R²)により、上記の理論値を実測で確認済み(random / reverse 両方の入力パターンで測定):

| 戦略 | 想定モデル | 実測 slope | R² |
|---|---|---|---|
| Simple | n² | 1.825〜1.944 | 0.999〜1.000 |
| Medium | n√n | 1.421〜1.451 | 0.999〜1.000 |
| Complex | n log₂n | 1.249 | 1.000 |
| Adaptive(各disorder帯) | 対応する戦略と一致 | 対応する戦略と一致 | 対応する戦略と一致 |

Adaptiveの各disorder帯(low/medium/high)の実測値が、対応するSimple/Medium/Complexの結果とほぼ一致していることは、閾値による自動切り替えが意図通り機能している根拠になる。

有限回の実測は理論値を「支持」できても「証明」はできない点に留意(サンプル数・n の範囲が限られるため)。

-----

## Error handling

| ケース | stdout | stderr | exit code |
|---|---|---|---|
| 非数値パラメータ | (なし) | `Error\n` | 1 |
| 重複した数値パラメータ | (なし) | `Error\n` | 1 |
| INT_MAX(2147483647)を超える値 | (なし) | `Error\n` | 1 |
| 引数なし | (なし) | (なし) | 0 |
| 既にソート済みの入力 | (なし、0手) | (なし) | 0 |

-----

## Resources

- [push_swap subject PDF](https://github.com/y1teng/42Cursus-Push_Swap) (42 School)
- [Man 1 shuf](https://linux.die.net/man/1/shuf)
- [zshとbashの単語分割の違い](https://qiita.com/mollifier/items/7fdbf15765ccf37f4881)
- 基数ソート・チャンクソートの一般的な計算量に関する参考資料(Wikipedia「選択ソート」「基数ソート」等)

### AI Usage

このプロジェクトでは Claude Code (Anthropic) を以下の用途で使用した:

- Norminette違反の修正方針の相談・診断(実装コードそのものの生成ではなく、違反箇所の特定とリファクタリング方針の提案)
- ディレクトリ構成(`src`/`include`/`obj`分割)やMakefile変更時に生じたビルドエラーの原因診断
- バグ報告(`op_reverse_rotate`の疑わしい挙動など)の検証・裏付けとなるテストの実行
- READMEの構成案・アルゴリズムの複雑性に関する数学的根拠の整理

コードの実装自体(ソートアルゴリズム、Makefileルールなど)は学習者自身が記述し、Claude Codeは主に相談・検証・診断・文章の整理に利用した。

-----

## Contributions

*(要確認: git log上の著者情報が実際の担当と一部ずれている可能性があるため、以下は暫定案。事実と異なる箇所は修正してください)*

### ayaito

- 入力パース・バリデーション(issue #15)
- Makefile整備(issue #23)、ディレクトリ構成(`src`/`include`/`obj`分割)の整理
- strategy dispatchの配線(issue #32)、`sort_medium`の結線(issue #33)
- 操作出力の分離設計・`--bench`実装(issue #38, #34)
- エラー出力の修正(issue #16)、Norminette対応(issue #19)
- GitHub issue管理、README作成

### ykomori

- 3要素ソート(`sort_three`)の実装
- 5要素ソート(`sort_five`)の実装

両学習者とも、担当箇所に限らずコードベース全体を説明できる状態にしてある。

-----

## 操作カウンタの設計 (issue #38)

### 何が課題だったか

`print_op()` は元々「標準出力に1行書く」だけの関数で、呼び出し元（`op_sa` など各 `op_*` 関数）と密結合していた。issue #34（`--bench`）や、ディフェンス当日に即興で求められうる `--count-only` 的なフラグなど、「出力の仕方だけを切り替えたい」要求が複数あり、そのたびに呼び出し側を触る必要がある構造だった。

### どう解決したか

「操作が起きた事実の記録（カウント）」と「それをどう出力するか」を `print_op()` の中で分離した。カウンタは `t_op` をインデックスにした配列（`counts[11]`）として持たせ、呼び出し側は今まで通り `print_op()` を呼ぶだけで、内部でカウントアップと出力の両方が行われる。

### なぜ `t_stack` にカウンタを持たせたか（グローバル変数にしなかった理由）

42 の Norm（Norminette）はファイルスコープの真のグローバル変数を原則禁止しており、可変な状態をグローバル変数として持つのは Norm 違反リスクが高い。一方 `t_stack` は元々すべての `op_*` 関数に引数として渡っている構造なので、そこにカウンタを載せれば新しい引数を増やさずに済む。つまり呼び出し側のシグネチャを一切変更せずに機能を拡張できる。

### なぜ `a.counts` と `b.counts` が同じバッファを指しているか

操作カウントは「a 側の操作」「b 側の操作」を別々に数えたいわけではなく、「操作種別ごとの総回数」を数えたい。そのため main で `a.counts` と `b.counts` に同一のポインタを代入し、a・b どちらの stack 経由で呼んでも同じ配列に集計されるようにしている。この設計により、`op_rr`/`op_rrr`/`op_ss` のように a と b の両方を操作する関数でも、`print_op` に渡すのは片方（`a`）だけでよい。

### この設計が今後どう活きるか

- issue #34（`--bench`）: このカウンタ配列をそのまま集計・stderr 出力に利用できる
- ディフェンス当日の `--count-only` 的な即興実装: 出力の有無の切り替えを `print_op` 内の 1 箇所に閉じ込められる

---

## [TODO: 提出前に削除] ライブコーディング練習メモ（--count-only）

### 課題内容（push_swap_review.md より）

`--count-only` フラグを追加し、操作列ではなく総手数だけを stdout に出力する。
例: `./push_swap --count-only 3 2 1` → `3` のみ出力（操作列は出さない）。
デモ込みで **10分以内** に完了させる必要がある。

### 触る場所の見当（コードは書かず、場所の確認だけ）

1. **フラグ解析** — `input_parsing.c` の `parse_flags()`。他の `--simple` 等と同じ並びに1行足す形。`t_options` に新しいフィールド（例: `count_only`）が要る。
2. **出力の抑制** — `print.c` の `print_op()`。既に `n->bench` で「stdout への個別出力を止めつつ `n->counts[op]++` だけ行う」仕組みがある。`count_only` の時も同様に個別出力を止めたい。既存の `bench` フラグをそのまま使い回すか、もう1つフラグを足すか要検討（挙動としては両方とも「個別出力を止めて集計だけする」で共通しているはず）。
3. **総手数の出力** — `main()` の末尾。`opt.count_only` が立っていれば `a.counts` の合計を1行 `ft_printf` で出す。合計を数えるロジック自体は `print_bench()`（`print.c`）の `total_ops` 集計とほぼ同じなので流用できる。

### 練習の進め方

- コードは書かず、上記3箇所を実際に開いて「ここを触ればいい」と指せるか確認する
- 「`t_stack.bench` をそのまま使うか、専用フラグを増やすか」を口頭で説明できるようにしておく
- 実際に書いてみる練習は使い捨てのブランチ/ディレクトリで行い、`main` には絶対に混ぜない

### デモ用テストケース例

```sh
./push_swap --count-only 3 2 1                              # -> 3
./push_swap --count-only 1 2 3 4 5                           # -> 0（既にソート済み）
./push_swap --count-only $(shuf -i 1-1000 -n 100) | wc -l    # 出力が1行だけであることの確認
```
