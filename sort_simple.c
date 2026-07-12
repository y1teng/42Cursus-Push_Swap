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
	int		buffer[4] = {3, 1, 2, 4};
	
//	a.data = buffer;
//	a.capacity = 4;
//	a.size = 4;
//	a.head = 2;
//
//	printf("after top: = %d\n",a.data[a.head % a.capacity]);
//	printf("before: head=%d\n", a.head);
//	idx = find_min_index(&a);
//	printf("idx = %d\n", idx);
//	printf("after: head=%d\n", a.head);
//	
//	printf("before top: = %d\n",a.data[a.head % a.capacity]);
	int steps = find_min_index(&a);
	while(steps){
		op_ra(&a);
		steps--;
	}
	op_pb;


	return (0);
}
