#include "push_swap.h"

void	op_reverse_rotate(t_stack *n)
{
	n->head = (n->head - 1 + n->capacity) % n->capacity;
}

void	op_rra(t_stack *a)
{
	op_reverse_rotate(a);
}

void	op_rrb(t_stack *b)
{
	op_reverse_rotate(b);
}

void	op_rrr(t_stack *a, t_stack *b)
{
	op_reverse_rotate(a);
	op_reverse_rotate(b);
}
