#include "push_swap.h"

void	op_reverse_rotate(t_stack *n)
{
	n->head = (n->head - 1 + n->capacity) % n->capacity;
}

void	rra(t_stack *a)
{
	op_reverse_rotate(a);
}

void	rrb(t_stack *b)
{
	op_reverse_rotate(b);
}

void	rrr(t_stack *a, t_stack *b)
{
	op_reverse_rotate(a);
	op_reverse_rotate(b);
}