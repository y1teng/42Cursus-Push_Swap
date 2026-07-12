#include "push_swap.h"

void	op_rotate(t_stack *n)
{
	int new_pos;
	new_pos = (n->head + n->size) % n->capacity;
	n->data[new_pos] = n->data[n->head];
	n->head = (n->head + 1) % n->capacity;
}

void	op_ra(t_stack *a)
{
	op_rotate(a);
}

void	op_rb(t_stack *b)
{
	op_rotate(b);
}

void	op_rr(t_stack *a, t_stack *b)
{
	op_rotate(a);
	op_rotate(b);
}
