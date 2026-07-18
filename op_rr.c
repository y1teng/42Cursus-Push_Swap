#include "push_swap.h"

void	op_reverse_rotate(t_stack *n)
{
	int bottom_pos = (n->head + n->size -1) % n->capacity;
	int new_head_pos = (n->head - 1 + n->capacity) % n->capacity;
	
	n->data[new_head_pos] = n->data[bottom_pos];
	n->head = new_head_pos;
}

void	op_rra(t_stack *a)
{
	op_reverse_rotate(a);
	print_op(a,OP_RRA);
}

void	op_rrb(t_stack *b)
{
	op_reverse_rotate(b);
	print_op(b,OP_RRB);
}

void	op_rrr(t_stack *a, t_stack *b)
{
	op_reverse_rotate(a);
	op_reverse_rotate(b);
	print_op(a,OP_RRR);
}
