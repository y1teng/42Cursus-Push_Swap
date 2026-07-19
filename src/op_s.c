/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   op_s.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:13:33 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:06:10 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	op_swap(t_stack *n)
{
	int	first;
	int	second;
	int	tmp;

	first = n->head;
	second = (n->head + 1) % n->capa;
	tmp = n->data[first];
	n->data[first] = n->data[second];
	n->data[second] = tmp;
}

void	op_sa(t_stack *a)
{
	op_swap(a);
	print_op(a, OP_SA);
}

void	op_sb(t_stack *b)
{
	op_swap(b);
	print_op(b, OP_SB);
}

void	op_ss(t_stack *a, t_stack *b)
{
	op_swap(a);
	op_swap(b);
	print_op(a, OP_SS);
}
