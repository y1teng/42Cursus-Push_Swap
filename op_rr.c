#include "push_swap.h"

void	op_reverse_rotate(t_stack *n)
{
	int new_pos;
	n->head = (n->head - 1 + n->capacity) % n->capacity;
	new_pos = (n->head + n->size) % n->capacity;
	n->data[new_pos] = n->data[n->head];
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
