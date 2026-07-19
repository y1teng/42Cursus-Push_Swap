/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op_r.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:11:36 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 19:18:28 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	op_rotate(t_stack *n)
{
	int	new_pos;

	new_pos = (n->head + n->size) % n->capacity;
	n->data[new_pos] = n->data[n->head];
	n->head = (n->head + 1) % n->capacity;
}

void	op_ra(t_stack *a)
{
	op_rotate(a);
	print_op(a, OP_RA);
}

void	op_rb(t_stack *b)
{
	op_rotate(b);
	print_op(b, OP_RB);
}

void	op_rr(t_stack *a, t_stack *b)
{
	op_rotate(a);
	op_rotate(b);
	print_op(a, OP_RR);
}
