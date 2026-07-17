#include "push_swap.h"

int	get_chunk(int value, int min, int max, int num_chunks)
{
	int	nm_c;

	nm_c = ((value - min) * num_chunks) / (max - min + 1);
	return (nm_c);
}

// int main()
//{
//	int ans;
//	ans = get_chunk(10,10,19,12);
//	printf("chunk = %d(except )", ans);
//	return (0);
//}
//入力
//- a, b: 対象の2つのスタック
//- chunk_idx: 今回bに移したいchunk番号
//- min, max, num_chunks: get_chunkを呼ぶために必要な情報
//
//出力
//- 戻り値なし（void）。副作用として、
// aの中にある「chunk_idxに属する要素」が全部bに移り、
// a・bの中身とサイズが変化する
//
//やること（繰り返しの構造）
//「見つける→移す」を、見つからなくなるまで繰り返します。
//
// 1. aのheadから順に、
// get_chunk(値, min, max, num_chunks) == chunk_idxとなる要素を探す
//（5要素ソートの「最小値を探す」ループとほぼ同じ形）
// 2. 見つかったら:
//  - その位置までaを最短方向に回転
//（sort_fiveで書いたop_rotate_shortestと同じ考え方）
//  - op_pb(b, a)でbに移す
// 3. aの全要素（a->size個）を見ても1つも見つからなかったら
//→ このchunkはもうaに残っていないので終了
//
//ポイント
// 1回のスキャンで1個しか移せません
//（1個移すとa->sizeが減ってheadも動くので、
//そのたびに最初から探し直す必要があります）。
//なので「探す→回転→pbする」のセット全体を、外側の繰り返し
//（whileなど）で包む形になります。
//
//まず「探す→見つかった位置を返す」部分と
//「回転してpbする」部分、どちらから書きますか？

void	push_chunk(t_stack *a, t_stack *b, int chunk_idx, int min, int max,
		int num_chunks)
{
	int	found;
	int	found_idx;
	int	count;

	found = 1;
	found_idx = -1;
	count = 0;
	if (!a || !b)
		return ;
	while (found)
	{
		found = 0;
		count = 0;
		while (count < a->size)
		{
			if (get_chunk(a->data[(a->head + count) % a->capacity], min, max,
					num_chunks) == chunk_idx)
			{
				found = 1;
				found_idx = count;
				break ;
			}
			count++;
		}
		if (found)
		{
			op_rotate_shortest(a, found_idx);
			op_pb(b, a);
		}
	}
}

void	distribute_chunks(t_stack *a, t_stack *b, int num_chunks)
{
	int	min_val;
	int	max_val;
	int	chunk_idx;

	chunk_idx = 0;
	get_min_max(a, &min_val, &max_val);
	while (chunk_idx < num_chunks)
	{
		push_chunk(a, b, chunk_idx, min_val, max_val, num_chunks);
		chunk_idx++;
	}
}

void	get_min_max(t_stack *a, int *min, int *max)
{
	int	min_val;
	int	max_val;
	int	count;

	if (!a)
		return ;
	count = 0;
	min_val = a->data[(a->head + count) % a->capacity];
	max_val = a->data[(a->head + count) % a->capacity];
	count = 0;
	while (count < a->size)
	{
		if (a->data[(a->head + count) % a->capacity] < min_val)
			min_val = a->data[(a->head + count) % a->capacity];
		else if (a->data[(a->head + count) % a->capacity] > max_val)
			max_val = a->data[(a->head + count) % a->capacity];
		count++;
	}
	*min = min_val;
	*max = max_val;
}

void	insert_back(t_stack *a, t_stack *b)
{
	int	min;
	int	count;
	int	min_idx;
	int	b_val;
	int	small_cnt;

	small_cnt = 0;
	while (b->size > 0)
	{
		b_val = b->data[b->head];
		small_cnt = 0;
		count = 0;
		while (count < a->size)
		{
			if (a->data[(a->head + count) % a->capacity] < b_val)
				++small_cnt;
			count++;
		}
		op_rotate_shortest(a, small_cnt);
		op_pa(a, b);
		min_idx = 0;
		count = 0;
		min = a->data[(a->head + count) % a->capacity];
		while (count < a->size)
		{
			if (a->data[(a->head + count) % a->capacity] < min)
			{
				min = a->data[(a->head + count) % a->capacity];
				min_idx = count;
			}
			count++;
		}
		op_rotate_shortest(a, min_idx);
	}
}

//
// int main(void)
//{
//      int arr[6] = {8, 3, 9, 1, 6, 2};
//      t_stack a;
//      int min, max;
//
//      a.data = arr;
//      a.capacity = 6;
//      a.size = 6;
//      a.head = 0;
//      get_min_max(&a, &min, &max);
//      printf("min=%d (expect 1), max=%d (expect 9)\n", min, max);
//		a.head = 2;
//		a.size = 4;
//
//      get_min_max(&a, &min, &max);
//      printf("min=%d (expect 1), max=%d (expect 9)\n", min, max);
//}

// #include "push_swap.h"
//  #include <stdio.h>
//
//  void distribute_chunks(t_stack *a, t_stack *b, int num_chunks);
//  void get_min_max(t_stack *a, int *min, int *max);
//  int get_chunk(int value, int min, int max, int num_chunks);
//
//  static void print_stack(const char *label, t_stack *s)
//  {
//        int i;
//
//        printf("%s (size=%d): ", label, s->size);
//        i = 0;
//        while (i < s->size)
//        {
//                printf("%d ", s->data[(s->head + i) % s->capacity]);
//                i++;
//        }
//        printf("\n");
//  }
//
//  int main(void)
//  {
//        int arr_a[9] = {8, 3, 9, 1, 6, 2, 7, 4, 5};
//        int arr_b[9] = {0};
//        t_stack a, b;
//        int min, max, i;
//
//        a.data = arr_a; a.capacity = 9; a.size = 9; a.head = 0;
//        b.data = arr_b; b.capacity = 9; b.size = 0; b.head = 0;
//
//        print_stack("before a", &a);
//        distribute_chunks(&a, &b, 3);
//        print_stack("after  a", &a);
//        print_stack("after  b", &b);
//
//        get_min_max(&a, &min, &max);
//        printf("b各要素のchunk番号: ");
//        i = 0;
//        while (i < b.size)
//        {
//                int v = b.data[(b.head + i) % b.capacity];
//                printf("%d(c%d) ", v, get_chunk(v, 1, 9, 3));
//                i++;
//        }
//        printf("\n");
//        return (0);
//  }
