#include "push_swap.h"

// void	op_visualize(t_stack *a, t_stack *b, char*label)
//{
//	int	a_top;
//	int	b_top;
//	puts("WWWWWWWWWWWWWWWWWWWWWWWWW");
//	printf("#a->size[%d], #b->size[%d]\n", a->size, b->size);
//	for (int i = 0; i < a->capacity; i++)
//	{
//		a_top = (a->head + i) % a->capacity;
//		printf("%s: a->head[%d], a->data[(%d)] = %d\n",label, a->head, a_top,
//			a->data[a_top]);
//	}
//	puts("------------------------");
//	for (int i = 0; i < b->capacity; i++)
//	{
//		b_top = (b->head + i) % b->capacity;
//		printf("%s: b->head[%d], b->data[(%d)] = %d\n",label, b->head, b_top,
//			b->data[b_top]);
//	}
//}

int main()
{
	int num = 1;
	int s_c = 5;
	a.capacity = 5;
	a.head = 0;
	int i = 0;
	int bf[s_c] = {num + i % s_c,
					num + 1 + (i % s_c),
					num + 2 + (i % s_c),
					num + 3 + (i % s_c),
					num + 4 + (i % s_c)
					};
	sort_five(&a,&b);
	printf("")
	return 0;
}


t_stack a, b をローカル変数として用意

a.data = {3, 1, 4, 5, 2} のような配列
a.capacity = a.size = 5, a.head = 0
b.data = 空の配列(要素数5ぶん確保), b.capacity = 5, b.size = 0, b.head = 0
sort_five(&a, &b) を呼ぶ
呼んだ後、a.dataを (a.head + i) % a.capacity の順で全部printfして、昇順になっているか目視確認
