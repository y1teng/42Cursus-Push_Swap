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

### そもそも「計算量($O(\cdot)$)」って何？

$n$個の数字を並び替えるとき「だいたいどれくらい大変か」を表す記号。例えば $O(n^2)$ は「$n$個のとき、だいたい $n^2$ 回くらいの作業が必要」という意味で、$n$を2倍にすると仕事量はだいたい $2^2=4$ 倍になる、というふうに使う。

### Simple戦略 — $O(n^2)$(選択ソート)

**やっていること**: 「残っている中で一番小さい数を見つけて、一番前に持ってくる」を全部終わるまで繰り返す。トランプを1枚ずつ端から探して並べていくイメージ。

**なぜ $O(n^2)$ になるか**: 1回目は$n$枚全部見て探し、2回目は$n-1$枚、3回目は$n-2$枚…と、見る枚数がだんだん減っていく。合計で見る回数は

$$n + (n-1) + (n-2) + \cdots + 1 = \frac{n(n+1)}{2}$$

という式になる。これは$n$が大きくなると、だいたい$n^2$に比例する大きさなので $O(n^2)$ と言う。

要素数が少ない場合や、ほぼ並び終わっている入力(disorder $< 0.2$)で使う。

### Medium戦略 — $O(n\sqrt{n})$(チャンク分割ソート)

**やっていること**: 数字の大きさで「グループ(バケット)」に分けて、グループごとにまとめて移動させる。1〜10のグループ、11〜20のグループ…のように仕分けするイメージ。

**なぜ $O(n\sqrt{n})$ になるか**: グループの数を $c$ 個とすると、手間は2種類ある。

$$A = c \times n \ (\text{仕分けにかかる手間}), \qquad B = \frac{n^2}{c} \ (\text{グループの中を並べ直す手間})$$

グループを増やす($c$を大きく)と$A$は増えるが$B$は減る、逆にグループを減らすと$A$は減るが$B$は増える、というトレードオフがある。ちょうどいい$c$を見つけたい。

**$c$の求め方**: $A$と$B$を掛け算すると

$$A \times B = (c \times n) \times \frac{n^2}{c} = n^3$$

$c$が消えて、**$c$の値に関係なく常に$n^3$という一定の値**になる。2つの正の数の掛け算の答えが一定なら、その合計が一番小さくなるのは2つが同じ値($A=B$)になったとき、という中学数学のルールがある(例えば$n=100$のとき、$c=1$なら合計$10100$、$c=10$なら合計$2000$、$c=50$なら合計$5200$と、$c=10=\sqrt{100}$のあたりが一番小さくなる)。

$A=B$を実際に解くと:

$$c \times n = \frac{n^2}{c} \quad\Rightarrow\quad c^2 = n \quad\Rightarrow\quad c = \sqrt{n}$$

これで$\sqrt{n}$が導かれた。このとき合計の手間は

$$A + B = c \times n + \frac{n^2}{c} = \sqrt{n} \times n + \frac{n^2}{\sqrt{n}} = n\sqrt{n} + n\sqrt{n} = 2n\sqrt{n} = O(n\sqrt{n})$$

なので $O(n\sqrt{n})$。だからチャンク数を $\lceil\sqrt{n}\rceil$ 個に決めている。

### Complex戦略 — $O(n\log n)$(基数ソート)

**やっていること**: 数字の大小を直接比べる代わりに、値を「順位」(0番目, 1番目, …)に変換してから、2進数の桁(ビット)を1桁ずつ見て「0のグループ」「1のグループ」に仕分ける。これを桁の数だけ繰り返す。

**なぜ $O(n\log n)$ になるか**: $n$個の順位を2進数で表すのに必要な桁数は

$$\text{桁数} = \lceil \log_2 n \rceil$$

1桁分の仕分けは$n$個全部を1回ずつ見るだけなので手間は$n$。これを桁数分繰り返すので、

$$\text{総手間} = n \times \lceil \log_2 n \rceil = O(n\log n)$$

普通の「比較して並べる」方式には $\Omega(n\log n)$ という下限があるが、Complexは値同士を比較しない仕組みなので、この下限には縛られない。

### Adaptive戦略 — 3つを自動で選ぶ

事前に入力がどれくらいバラバラか(disorder、$0$〜$1$の値)を計算し、こう選ぶ:

$$
\text{strategy} =
\begin{cases}
\text{Simple} & (\text{disorder} < 0.2) \\
\text{Medium} & (0.2 \le \text{disorder} < 0.5) \\
\text{Complex} & (\text{disorder} \ge 0.5)
\end{cases}
$$

サイズが2, 3, 5の入力は、この閾値に関係なく専用の小さいルーチン(直接swap / `sort_three` / `sort_five`)を使うため、常にSimple相当として扱う。

### 本当にその通りになっているか、実際に測ってみた

理論上、サイズ $n$ を2倍にすると:

- Simple($O(n^2)$): 仕事量は $2^2 = 4$ 倍になるはず
- Medium($O(n\sqrt{n})$): 仕事量は $2\sqrt{2} \approx 2.8$ 倍になるはず
- Complex($O(n\log n)$): 仕事量は2倍よりちょっと多いくらいになるはず($n$が大きいほど「ちょっと多い」の部分は小さくなる)

実際に `push_swap` をサイズ $16, 32, 64, 128, 256$(2倍ずつ)で動かして、出力される命令の数を数えてみた(乱数入力、各サイズ3回の平均):

| サイズ $n$ | Simple | Medium | Complex |
|---|---|---|---|
| 16 | 85 | 68 | 96 |
| 32 | 306 | 221 | 240 |
| 64 | 1151 | 566 | 576 |
| 128 | 4481 | 1573 | 1344 |
| 256 | 15994 | 4239 | 3072 |

サイズが2倍になるごとの倍率:

| $n$の変化 | Simple(理論 $\approx 4$) | Medium(理論 $\approx 2.8$) | Complex(理論 $\approx 2.3$〜$2.5$) |
|---|---|---|---|
| $16 \to 32$ | 3.6 | 3.3 | 2.5 |
| $32 \to 64$ | 3.8 | 2.6 | 2.4 |
| $64 \to 128$ | 3.9 | 2.8 | 2.3 |
| $128 \to 256$ | 3.6 | 2.7 | 2.3 |

理論で予想した倍率と、実際に測った倍率がほぼ一致している。つまり「Simpleは本当に$O(n^2)$っぽい動きをしていて、Medium・Complexはそれぞれもっと効率の良い動きをしている」ということが実験で確かめられた。

Adaptiveは disorder に応じてSimple/Medium/Complexのどれかを選ぶだけなので、選ばれた戦略と同じ動きになるはず。実際に測ってもその通りになっていることを確認済み。

注意: これは「何回か試してみたら理論通りだった」という実験結果であり、数学的な「証明」ではない(試した回数・サイズの範囲には限りがあるため)。

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
- Gitの管理(ブランチのマージ、issueのクローズ・コメント記載、コミット作成)
- タスク・スケジュール管理(GitHub issueの優先順位整理、進捗の追跡)

-----

## Contributions

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
