
#include "push_swap.h"

void	op_rotate_shortest(t_stack *n, int steps)
{
	int	count;

	count = n->size - steps;
	if (steps <= n->size - steps)
	{
		while (steps)
		{
			op_ra(n);
			steps--;
		}
	}
	else
	{
		while (count)
		{
			op_rra(n);
			count--;
		}
	}
}

void	sort_five(t_stack *a, t_stack *b)
{
	int	i;
	int	min;
	int	count;
	int	min_idx;

	i = 0;
	min = a->data[i];
	count = 0;
	if (!a)
		return ;
	while (i < 2)
	{
		min_idx = 0;
		min = a->data[(a->head + count) % a->capacity];
		count = 0;
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
			op_pb(b, a);
		i++;
	}
	i = 0;
	sort_three(a);
	while (i < 2)
	{
		op_pa(a, b);
		i++;
			}

}
