/**
 * @mainpage push_swap
 *
 * 2つのスタック（a, b）と11種類の操作だけを使って整数列をソートする。
 *
 * 各スタックは t_stack （固定長の循環バッファ）として表現されている。
 * `head` が「今どこが先頭(top)か」を指すインデックスで、実際の値は動かさず
 * `head` を動かすことで push / rotate を O(1) で行う。詳しくは
 * @ref push_swap.h の t_stack を参照。
 *
 * 処理の流れ： main() が引数をパースして t_stack a/b を組み立て、
 * sort_simple() （選択ソート方式）に処理を渡す。各操作関数
 * (op_sa, op_pa, op_ra, op_rra, ...) は状態を変更したあと必ず
 * print_op() で操作名を1行 stdout に出す。
 *
 * @note op_reverse_rotate() は `size == capacity`（スタックが満杯）の間は
 * 正しく動くが、`size < capacity` の部分状態ではまだ検証・修正できていない
 * （op_rotate() で見つかったのと同種の「ゴミ混入」バグの疑いがある）。
 */

#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft/libft.h"
# include "libftprintf/ft_printf.h"
# include <limits.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
/** ソート戦略。実行時に `--simple` 等のフラグで選ぶ（未指定なら ADAPTIVE）。 */
typedef enum e_strategy
{
	ADAPTIVE,
	SIMPLE,
	MEDIUM,
	COMPLEX
}				t_strategy;

/** print_op() が stdout に出す11種類の操作名に対応する識別子。 */
typedef enum e_op
{
	OP_SA,
	OP_SB,
	OP_SS,
	OP_PA,
	OP_PB,
	OP_RA,
	OP_RB,
	OP_RR,
	OP_RRA,
	OP_RRB,
	OP_RRR
}				t_op;

/** parse_flags() が argv から読み取った実行オプション。 */
typedef struct s_options
{
	t_strategy	strategy;
	int			bench;
	/** フラグではない最初の argv インデックス（＝整数列の開始位置）。 */
	int			num_start;
}				t_options;

/**
 * @brief スタック a / b を表す固定長の循環バッファ。
 *
 * 有効な要素は `data[(head + i) % capacity]`（`i` は 0 から `size - 1`）で
 * 読む。`capacity` は初期入力の個数から決まり、ソート中ずっと変わらない。
 * `size` が `capacity` より小さいとき、有効範囲の外側の `data` には
 * 直前まで使われていた古い値（ゴミ）が残っている点に注意。
 */
typedef struct s_stack
{
	int			*data;
	/** 確保済みの配列長。ソート開始時の要素数で固定。 */
	int			capacity;
	/** 現在の有効要素数（push/popのたびに増減する）。 */
	int			size;
	/** 現在の先頭(top)を指すインデックス。ra/rraで動く。 */
	int			head;
}				t_stack;

/**
 * @brief Simple 戦略（選択ソート方式）で a をソートし、b に受け皿として使う。
 *
 * a の最小値を rotate で先頭に持ってきては b に push、を size 回繰り返した
 * あと、b から a へ全部 push し直す。操作数の最小化はスコープ外で、
 * 小さい入力で正しく動くことを優先している。
 */
void			sort_simple(t_stack *a, t_stack *b);

// add 3 sort
void			sort_three(t_stack *a);
void    sort_five(t_stack *a, t_stack *b);
/**
 * @brief src の先頭を dst の先頭へ移す（pa/pb の実体）。
 * src が空なら何もしない。
 */
void			op_push(t_stack *dst, t_stack *src);
/** @brief `pa`: b の先頭を a の先頭に push する。 */
void			op_pa(t_stack *dst, t_stack *src);
/** @brief `pb`: a の先頭を b の先頭に push する。 */
void			op_pb(t_stack *dst, t_stack *src);

/**
 * @brief スタック n を1つ左回転する（ra/rb の実体）。
 *
 * 先頭の値を、有効範囲の外にはみ出す1マスへコピーしてから head を
 * 進める。この「コピーしてから head を進める」順番は、部分状態
 * （size < capacity）のときにはみ出すマスへ古いゴミ値が残る不具合の
 * 修正として導入された。アルゴリズムに依存しない汎用的な修正。
 */
void			op_rotate(t_stack *n);
/** @brief `ra`: a を1つ左回転する。 */
void			op_ra(t_stack *a);
/** @brief `rb`: b を1つ左回転する。 */
void			op_rb(t_stack *b);
/** @brief `rr`: ra と rb を同時に行う。 */
void			op_rr(t_stack *a, t_stack *b);

/**
 * @brief スタック n を1つ右回転する（rra/rrb の実体）。
 *
 * @warning op_rotate() と同じ「はみ出しマスのゴミ混入」問題への対処が
 * 未検証。`size == capacity`（満杯）のときは正しく動くが、
 * `size < capacity` の部分状態では末尾ではなく無効なマスの値を
 * 先頭に読み込んでしまう可能性がある（要修正）。
 */
void			op_reverse_rotate(t_stack *n);
/** @brief `rra`: a を1つ右回転する。 */
void			op_rra(t_stack *a);
/** @brief `rrb`: b を1つ右回転する。 */
void			op_rrb(t_stack *b);
/** @brief `rrr`: rra と rrb を同時に行う。 */
void			op_rrr(t_stack *a, t_stack *b);

/** @brief スタック n の先頭2要素を入れ替える（sa/sb の実体）。 */
void			op_swap(t_stack *n);
/** @brief `sa`: a の先頭2要素を入れ替える。 */
void			op_sa(t_stack *a);
/** @brief `sb`: b の先頭2要素を入れ替える。 */
void			op_sb(t_stack *b);
/** @brief `ss`: sa と sb を同時に行う。 */
void			op_ss(t_stack *a, t_stack *b);

/** @brief 操作 op に対応する名前（"sa" 等）を1行 stdout に出力する。 */
void			print_op(t_op op);

#endif
