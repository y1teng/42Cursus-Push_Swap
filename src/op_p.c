/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op_p.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:11:32 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 19:18:43 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

void	op_pa(t_stack *dst, t_stack *src)
{
	op_push(dst, src);
	print_op(dst, OP_PA);
}

void	op_pb(t_stack *dst, t_stack *src)
{
	op_push(dst, src);
	print_op(dst, OP_PB);
}
