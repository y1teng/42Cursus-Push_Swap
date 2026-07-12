#include "push_swap.h"
#include <stdio.h>

static int	sort_find_min_index(t_stack *a)
{
	int	count;
	int	i;
	int	min;
	int	min_idx;

	count = 0;
	min_idx = 0;
	i = a->head;
	min = a->data[i];
	if (!a)
		return (-1);
	while (count < a->size)
	{
		i = (a->head + count) % a->capacity;
		if (min > a->data[i])
		{
			min = a->data[i];
			min_idx = count;
		}
		count++;
	}
	return (min_idx);
}

//void	op_visualize(t_stack *a, t_stack *b, char*label)
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

void sort_simple(t_stack *a,t_stack *b)
{
	int		i;
	int		steps;
	int		origin_size;

	origin_size = a->size;
	i = 0;
	
	while (i < origin_size)
	{
		steps = sort_find_min_index(a);
		while (steps)
		{
			op_ra(a);
			steps--;
		}
		op_pb(b, a);
		i++;
	}
	i = 0;
	while (i < origin_size)
	{
		op_pa(a, b);
		i++;
	}
}
//int	main(void)
//{
//	t_stack	a;
//	t_stack	b;
//	int		buffer_a[4] = {3, 1, 2, 4};
//	int		buffer_b[4] = {0};
//	int		i;
//	int		steps;
//	int		origin_size;
//
//	a.data = buffer_a;
//	a.capacity = 4;
//	a.size = 4;
//	a.head = 2;
//	b.data = buffer_b;
//	b.capacity = 4;
//	b.size = 0;
//	b.head = 2;
//	origin_size = a.size;
//	i = 0;
//	op_visualize(&a, &b, "before");
//	
//	while (i < origin_size)
//	{
//		steps = find_min_index(&a);
//		while (steps)
//		{
//			op_ra(&a);
//			steps--;
//		}
//		op_pb(&b, &a);
//		i++;
//	}
//	i = 0;
//	while (i < origin_size)
//	{
//		op_pa(&a, &b);
//		i++;
//	}
//	op_visualize(&a, &b, "after");
//	return (0);
//}

//int main()
//{
//		t_stack a;
//	int buffer[4] = {3, 1, 2, 4};
//
//	a.data = buffer;
//	a.capacity = 4;
//	a.size = 2;
//	a.head = 3;
//
//	printf("--- raを押す前 ---\n");
//	for (int i = 0; i < 4; i++)
//		printf("data[%d] = %d\n", i, a.data[i]);
//
//	op_ra(&a);
//
//	printf("--- raを押した後 ---\n");
//	for (int i = 0; i < 4; i++)
//		printf("data[%d] = %d\n", i, a.data[i]);
//
//	printf("head = %d\n", a.head);
//	return (0);
//}
