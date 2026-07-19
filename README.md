*This project has been created as part of the 42 curriculum by ykomori, ayaito.*

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

### 実測による裏付け(できるだけ簡単に説明)

`O(n²)` などの記号は「入力のサイズ `n` を2倍にしたら、仕事の量が何倍になるか」を表している。

- `O(n²)`(Simple): サイズを2倍にすると、仕事量は **2² = 4倍**になる
- `O(n√n)`(Medium): サイズを2倍にすると、仕事量は **2×√2 ≈ 2.8倍**になる
- `O(n log n)`(Complex): サイズを2倍にすると、仕事量は **2倍よりちょっと多いくらい**になる(サイズが大きいほど「ちょっと多い」の部分は小さくなる)

これが本当かどうか、実際に `push_swap` を色々なサイズ(16個, 32個, 64個, ... と2倍ずつ増やしながら)動かして、出力される命令の数を数えてみた(乱数入力、各サイズ3回の平均):

| サイズ n | Simple(実際の手数) | Medium(実際の手数) | Complex(実際の手数) |
|---|---|---|---|
| 16 | 85 | 68 | 96 |
| 32 | 306 | 221 | 240 |
| 64 | 1151 | 566 | 576 |
| 128 | 4481 | 1573 | 1344 |
| 256 | 15994 | 4239 | 3072 |

サイズを2倍にするたびに手数が何倍になったかを計算すると:

| n の変化 | Simple(理論:約4倍) | Medium(理論:約2.8倍) | Complex(理論:2.3〜2.5倍) |
|---|---|---|---|
| 16→32 | 3.6倍 | 3.3倍 | 2.5倍 |
| 32→64 | 3.8倍 | 2.6倍 | 2.4倍 |
| 64→128 | 3.9倍 | 2.8倍 | 2.3倍 |
| 128→256 | 3.6倍 | 2.7倍 | 2.3倍 |

理論で予想した倍率(4倍 / 2.8倍 / 2.3〜2.5倍)と、実際に測った倍率がほぼ一致している。つまり「Simpleは本当にO(n²)っぽい動きをしていて、Medium・Complexはそれぞれもっと効率の良い動きをしている」ということが実験で確かめられた。

Adaptiveは、乱雑さ(disorder)に応じてSimple/Medium/Complexのどれかを選ぶだけなので、選ばれた戦略と同じ動きになるはず。実際に測ってもその通りになっていることを確認済み。

注意: これはあくまで「何回か試してみたら理論通りだった」という実験結果であり、数学的な「証明」ではない(試した回数やサイズの範囲には限りがあるため)。

-----

## ファイル・関数リファレンス

### `push_swap.h`

役割: 全ファイルで共有する型定義とプロトタイプ宣言。

- `t_strategy`(enum): `ADAPTIVE`/`SIMPLE`/`MEDIUM`/`COMPLEX`
- `t_op`(enum): 11種類の操作(`sa`〜`rrr`)を表すインデックス
- `t_options`: `strategy`, `bench`, `num_start`
- `t_stack`: `data`, `capa`, `size`, `head`, `counts`, `bench`
- `t_chunk_range`: `min`, `max`, `num_chunks`(medium戦略専用)

### `main.c` — エントリーポイントとソート全体の流れの制御

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `stack_init` | `t_stack *s, int *data, size_t capa, size_t size` | なし | スタックの初期状態(データ配列・容量・サイズ・`head=0`・`bench=0`)をセットする |
| `parse_and_build` | `int argc, char **argv, t_options *opt, t_stack *a` | `int`(0=成功, 1=失敗) | フラグ解析→配列構築→検証→`a`の初期化までを行う。失敗時は`Error`を出力しリソースを解放する |
| `dispatch_sort`(static) | `t_stack *a, t_stack *b, t_strategy effective, double disorder` | なし | サイズと戦略に応じて`sort_three`/`sort_five`/`sort_simple`/`sort_medium`/`sort_complex`のいずれかを呼び分ける |
| `main` | `int argc, char **argv` | `int`(exit code) | 全体のオーケストレーション: パース→初期化→disorder計算→戦略決定→ソート→(bench出力)→解放 |

### `input_parsing.c` — CLI引数(フラグ・数値文字列)の解析

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `parse_flags` | `int argc, char **argv, t_options *opt` | なし | `--simple`等のフラグを判定し`opt`にセット、最初の非フラグ位置を`num_start`に記録 |
| `parse_sign`(static) | `const char **s` | `int`(1 or -1) | 先頭の`+`/`-`を読み取り符号を返す、ポインタを1つ進める |
| `parse_int` | `const char *s, int *out` | `int`(0=成功, 1=失敗) | 文字列を整数に変換。非数値・空文字・INT範囲外は失敗として1を返す |
| `parse_atoi_array` | `int *arr, char **argv, int size` | `int`(0=成功, 1=失敗) | 文字列配列の各要素に`parse_int`を適用、1つでも失敗したら1を返す |

### `input_build.c` — argvから整数配列を構築する

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `build_arr_from_string`(static) | `char *str, int *out_size` | `int *`(NULL=失敗) | 空白区切りの1引数文字列を`ft_split`でトークン化し、配列に変換 |
| `build_arr` | `char **argv, int start, int argc, int *out_size` | `int *`(NULL=失敗) | argvが1個の文字列か複数の数値argvかを判定し、どちらかの構築処理を呼ぶ |

### `input_validate.c` — 構築済み配列の妥当性検証

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `validate_arr` | `int *arr, int size` | `int`(0=OK, 1=重複あり) | 全ペアを比較し重複値がないか確認する(O(n²)) |

### `input_strategy.c` — 実行時に使う戦略の決定

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `resolve_strategy` | `t_strategy strategy, int size, double disorder` | `t_strategy` | 明示指定ならそのまま返す。`ADAPTIVE`かつ`size`が2,3,5なら`SIMPLE`、それ以外は`disorder`の閾値で判定 |

### `disorder_metric.c` — 入力の乱雑さ(disorder)の計算

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `count_mismatches`(static) | `t_stack *a` | `double` | 全ペアを比較し「逆転しているペア数」を数える |
| `disorder_compute` | `t_stack *a` | `double`(0.0〜1.0) | 逆転ペア数 ÷ 総ペア数(`n(n-1)/2`)。`size<2`なら0を返す |

### `op_p.c` — push操作(`pa`/`pb`)

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `op_push` | `t_stack *dst, t_stack *src` | なし | `src`の先頭を`dst`の先頭へ移動する実処理(循環バッファのhead/size更新) |
| `op_pa` | `t_stack *dst, t_stack *src` | なし | `op_push`を呼び、`OP_PA`として記録・出力 |
| `op_pb` | `t_stack *dst, t_stack *src` | なし | `op_push`を呼び、`OP_PB`として記録・出力 |

### `op_r.c` — 通常回転(`ra`/`rb`/`rr`)

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `op_rotate` | `t_stack *n` | なし | 先頭要素を末尾へ移動する実処理 |
| `op_ra` | `t_stack *a` | なし | `a`を回転し`OP_RA`を記録 |
| `op_rb` | `t_stack *b` | なし | `b`を回転し`OP_RB`を記録 |
| `op_rr` | `t_stack *a, t_stack *b` | なし | `a`と`b`を同時に回転し`OP_RR`を記録 |

### `op_rr.c` — 逆回転(`rra`/`rrb`/`rrr`)

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `op_reverse_rotate` | `t_stack *n` | なし | 末尾要素を先頭へ移動する実処理 |
| `op_rra` | `t_stack *a` | なし | `a`を逆回転し`OP_RRA`を記録 |
| `op_rrb` | `t_stack *b` | なし | `b`を逆回転し`OP_RRB`を記録 |
| `op_rrr` | `t_stack *a, t_stack *b` | なし | `a`と`b`を同時に逆回転し`OP_RRR`を記録 |

### `op_s.c` — swap操作(`sa`/`sb`/`ss`)

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `op_swap` | `t_stack *n` | なし | 先頭2要素を入れ替える実処理 |
| `op_sa` | `t_stack *a` | なし | `a`をswapし`OP_SA`を記録 |
| `op_sb` | `t_stack *b` | なし | `b`をswapし`OP_SB`を記録 |
| `op_ss` | `t_stack *a, t_stack *b` | なし | `a`と`b`を同時にswapし`OP_SS`を記録 |

### `sort_three.c` — 3要素専用ソート

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `sort_three` | `t_stack *a` | なし | 3要素の大小関係を全パターン(6通り)判定し、最小手数の操作列を直接実行する |

### `sort_five.c` — 5要素専用ソート + 最短回転の共通処理

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `op_rotate_shortest` | `t_stack *n, int steps` | なし | 目的位置(`steps`)まで、前方回転と逆回転のうち手数が短い方を選んで実行する。他のsort系ファイルからも共通で使われる |
| `sort_five` | `t_stack *a, t_stack *b` | なし | 最小の2要素を`find_min_idx`で見つけて`b`へ退避 → 残り3要素を`sort_three` → 2要素を`a`へ戻す |

### `sort_simple.c` — 選択ソート(O(n²))

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `sort_find_min_index`(static) | `t_stack *a` | `int`(現在のheadを基準にした相対位置) | `a`内の最小値の位置を線形探索で見つける |
| `sort_simple` | `t_stack *a, t_stack *b` | なし | 最小値を探して`b`へpushをn回繰り返し、その後全部`a`へpush-back |

### `sort_medium.c` — チャンク分割ソート(O(n√n))のエントリーポイント

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `isqrt`(static) | `int n` | `int`(整数平方根、切り捨て) | チャンク数(`⌈√n⌉`)を決めるための整数平方根計算 |
| `sort_medium` | `t_stack *a, t_stack *b` | なし | チャンク数を決め、`distribute_chunks`→`insert_back`の順に呼ぶ |

### `sort_medium_chunk.c` — 値をチャンク(バケット)に分けて`b`へ移送する処理

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `stack_at` | `t_stack *a, int idx` | `int`(値) | 循環バッファの添字計算`(head+idx)%capa`を隠蔽するアクセサ。他ファイルからも共通利用 |
| `get_chunk`(static) | `int value, t_chunk_range range` | `int`(チャンク番号) | 値がどのチャンクに属するかを線形スケーリングで計算 |
| `get_min_max`(static) | `t_stack *a, int *min, int *max` | なし | `a`内の最小値・最大値を求める(チャンク範囲の元になる) |
| `push_chunk`(static) | `t_stack *a, t_stack *b, int chunk_idx, t_chunk_range range` | なし | 指定チャンクに属する要素を`a`から見つけて`b`へ移送する(見つからなくなるまで繰り返す) |
| `distribute_chunks` | `t_stack *a, t_stack *b, int num_chunks` | なし | 全チャンク番号について`push_chunk`を呼び、`a`の全要素を`b`へ分配する |

### `sort_medium_insert.c` — `b`から`a`へ、値の大小関係を保ちながら戻す処理

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `count_smaller`(static) | `t_stack *a, int val` | `int`(個数) | `a`内で`val`より小さい要素の個数を数える(挿入位置の計算に使う) |
| `find_min_idx` | `t_stack *a` | `int`(相対位置) | `a`内の最小値の位置を探す。`sort_five`からも共通利用 |
| `insert_back` | `t_stack *a, t_stack *b` | なし | `b`の先頭要素を、`a`内の正しい位置(小さい要素の数だけ回転)へ挿入する、を`b`が空になるまで繰り返す |

### `sort_complex.c` — LSD基数ソート(O(n log n))

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `sort_array`(static) | `int *arr, int size` | なし | 生配列をバブルソートで昇順に並べる(`ranks_convert`の前処理用) |
| `find_rank`(static) | `int *sorted, int size, int value` | `int`(順位、見つからなければ-1) | ソート済み配列内で`value`が何番目かを線形探索する |
| `ranks_convert` | `int *arr, int size` | なし | `a->data`の各値を「0〜n-1の順位」に置き換える(以降ビット比較のみで済むようにする前処理) |
| `count_bits`(static) | `int size` | `int`(必要ビット数) | 順位を表現するのに必要なビット数(`⌈log2(size)⌉`)を計算する |
| `sort_complex` | `t_stack *a, t_stack *b` | なし | 順位変換後、下位ビットから順に`a`/`b`へ振り分けるLSD基数ソート本体 |

### `print.c` — 操作の出力・カウント、`--bench`統計出力

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `strategy_name`(static) | `t_strategy strategy` | `const char *` | 戦略名(`Adaptive`等)の文字列を返す |
| `complexity_name`(static) | `t_strategy strategy` | `const char *` | 複雑性クラス(`O(n²)`等)の文字列を返す |
| `print_disorder`(static) | `double disorder` | なし | disorder値を`XX.YY%`形式でstderrへ出力する |
| `print_bench` | `t_options opt, t_stack a, double disorder, t_strategy effective` | なし | disorder・戦略名・総手数・操作種別ごとの回数をまとめてstderrへ出力する |
| `print_op` | `t_stack *n, t_op op` | なし | **全op_\*関数から呼ばれる中核関数**。`bench`が立っていなければ操作名を1行stdoutへ出力し、必ずカウンタを+1する |

### `print_ft_dprintf.c` — 任意のファイルディスクリプタへ書けるprintf(`--bench`用に自作)

| 関数 | 引数 | 戻り値 | 説明 |
|---|---|---|---|
| `ft_dprintf_putstr_fd`(static) | `char *s, int fd` | `int`(書き込みバイト数) | 文字列をそのまま`write`する |
| `ft_dprintf_putnbr_fd`(static) | `int n, int fd` | `int`(書き込みバイト数) | 整数を文字列化して`write`する(負数対応) |
| `ft_dprintf_core`(static) | `int fd, const char *fmt, va_list ap` | `int`(合計書き込みバイト数) | 書式文字列を1文字ずつ処理し、`%s`/`%d`ならそれぞれの関数へ委譲、それ以外はそのまま出力 |
| `ft_dprintf` | `int fd, const char *fmt, ...` | `int`(合計書き込みバイト数、`fmt`がNULLなら-1) | 可変長引数を受け取り`ft_dprintf_core`へ渡す公開インターフェース |

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
- [計算量オーダー(Big-O)の解説 (ios-net blog)](https://www.ios-net.co.jp/blog/20251015-6096/)
- [push_swap理解用スライド](https://docs.google.com/presentation/d/1hBR9R5TOrpS7cJvRjidt7Yv9Yfc3li5lRuAVypkql9E/edit?slide=id.p28#slide=id.p28)
- [Circular buffer (Wikipedia)](https://en.wikipedia.org/wiki/Circular_buffer) — `t_stack` の `head`/`capacity`を使った循環バッファ的な添字管理の元ネタ
- [座標圧縮の解説](https://algo-logic.info/coordinate-compress/) — `sort_complex.c`の`ranks_convert()`(値を順位に変換する前処理)の考え方
- [基数ソート入門](https://tukumolog.com/radix-sort-introduction/) — `sort_complex.c`のLSD基数ソートの元ネタ

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
- 基数ソート(`sort_complex`)の実装

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
