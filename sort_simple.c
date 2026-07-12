#include "push_swap.h"
#include <stdio.h>

int	find_min_index(t_stack *a)
{
	int count = 0;
	int	i;
	int	min;
	int min_idx = 0;
	
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

int	main(void)
{
	t_stack	a;
	int		buffer_a[4] = {3, 1, 2, 4};
	
	a.data = buffer_a;
	a.capacity = 4;
	a.size = 4;
	a.head = 2;

	t_stack b;
	int	buffer_b[4] = {0};

	b.data = buffer_b;
	b.capacity = 4;
	b.size = 0;
	b.head = 2;

	int a_top;
	int b_top;
//	printf("after top: = %d\n",a.data[a.head % a.capacity]);
//	printf("before: head=%d\n", a.head);
//	idx = find_min_index(&a);
//	printf("idx = %d\n", idx);
//	printf("after: head=%d\n", a.head);
//	y
//	printf("before top: = %d\n",a.data[a.head % a.capacity]);
	printf("a.size[%d]\n",a.size);
	for(int i = 0;i < a.capacity;i++)
	{
		a_top = (a.head + i) % a.capacity;
		printf("before: a.head[%d], a.data[(%d)] = %d\n",a.head ,a_top,
		a.data[a_top]);
	}	

	for(int i = 0;i < b.capacity;i++)
	{
		b_top = (b.head + i) % b.capacity;
		printf("before: b.head[%d], b.data[(%d)] = %d\n",b.head, b_top,
		b.data[b_top]);
	}

	int steps = find_min_index(&a);
	printf("steps[%d]\n", steps);

	while(steps){
		op_ra(&a);
		a_top = (a.head + steps) % a.capacity;
		printf("while: a.head[%d], a.data[(%d)] = %d\n",a.head ,a_top,
		a.data[steps]);
		steps--;
	}
		a_top = (a.head + steps) % a.capacity;
		printf("while: a.head[%d], a.data[(%d)] = %d\n",a.head ,a_top,
		a.data[steps]);
	op_pb(&b, &a);
	
	for(int i = 0;i < a.capacity;i++)
	{
		a_top = (a.head + i) % a.capacity;
		printf("after: a.head[%d], a.data[(%d)] = %d\n", a.head, a_top,
		a.data[a_top]);
	}

	for(int i = 0;i < b.capacity;i++)
	{
		b_top = (b.head + i) % b.capacity;
		printf("after: b.head[%d], b.data[(%d)] = %d\n", b.head, b_top,
		b.data[b_top]);
	}
	printf("a.size[%d]\n",a.size);

	return (0);
}
