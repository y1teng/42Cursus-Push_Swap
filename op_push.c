#include "push_swap.h"

void	op_push(t_stack *dst, t_stack *src)
{
	int	tmp;

	if (!src->size)
		return ;
	tmp = src->data[src->head];
	dst->head = (dst->head - 1 + dst->capacity) % dst->capacity;
	src->head = (src->head + 1) % src->capacity;
	dst->data[dst->head] = tmp;
	dst->size++;
	src->size--;
}

void	pa(t_stack *dst, t_stack *src)
{
	op_push(dst, src);
}

void	pb(t_stack *dst, t_stack *src)
{
	op_push(dst, src);
}