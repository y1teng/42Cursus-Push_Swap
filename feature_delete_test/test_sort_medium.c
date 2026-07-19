

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
