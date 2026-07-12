#include "push_swap.h"

void	op_rotate(t_stack *n)
{
	n->head = (n->head + 1) % n->capacity;
}

void	ra(t_stack *a)
{
	op_rotate(a);
}

void	rb(t_stack *b)
{
	op_rotate(b);
}

void	rr(t_stack *a, t_stack *b)
{
	op_rotate(a);
	op_rotate(b);
}