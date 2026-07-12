#include "push_swap.h"
#include <stdio.h>

int	find_min_index(t_stack *a)
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

void	op_visualize(t_stack *a, t_stack *b)
{
	int	a_top;
	int	b_top;

	printf("a->size[%d]\n", a->size);
	for (int i = 0; i < a->capacity; i++)
	{
		a_top = (a->head + i) % a->capacity;
		printf("before: a->head[%d], a->data[(%d)] = %d\n", a->head, a_top,
			a->data[a_top]);
	}
	for (int i = 0; i < b->capacity; i++)
	{
		b_top = (b->head + i) % b->capacity;
		printf("before: b->head[%d], b->data[(%d)] = %d\n", b->head, b_top,
			b->data[b_top]);
	}
}

int	main(void)
{
	t_stack	a;
	t_stack	b;
	int		buffer_a[4] = {3, 1, 2, 4};
	int		buffer_b[4] = {0};
	int		i;
	int		steps;
	int		origin_size;

	a.data = buffer_a;
	a.capacity = 4;
	a.size = 4;
	a.head = 2;
	b.data = buffer_b;
	b.capacity = 4;
	b.size = 0;
	b.head = 2;
	origin_size = a.size;
	i = 0;
	op_visualize(&a, &b);
	
	while (i < origin_size)
	{
		steps = find_min_index(&a);
		while (steps)
		{
			op_ra(&a);
			steps--;
		}
		op_pb(&b, &a);
		i++;
	}
	op_visualize(&a, &b);
	i = 0;
	while (i < origin_size)
	{
		op_pa(&a, &b);
		i++;
	}
	op_visualize(&a, &b);
	return (0);
}
