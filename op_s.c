#include "push_swap.h"

void	op_swap(t_stack *n)
{
	int	first;
	int	second;
	int	tmp;

	first = n->head;
	second = (n->head + 1) % n->capacity;
	tmp = n->data[first];
	n->data[first] = n->data[second];
	n->data[second] = tmp;
}

void	op_sa(t_stack *a)
{
	op_swap(a);
}

void	op_sb(t_stack *b)
{
	op_swap(b);
}
void	op_ss(t_stack *a, t_stack *b)
{
	op_swap(a);
	op_swap(b);
}
