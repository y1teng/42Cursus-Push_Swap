/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_three.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykomori <ykomori@student.42tokyo.jp>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 13:23:52 by ykomori           #+#    #+#             */
/*   Updated: 2026/07/14 13:28:35 by ykomori          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	sort_three(t_stack *a)
{
	int	va;
	int	vb;
	int	vc;

	va = a->data[a->head];
	vb = a->data[(a->head + 1) % a->capacity];
	vc = a->data[(a->head + 2) % a->capacity];
	if (va < vb && vb < vc)
		return ;
	else if (vb < va && va < vc)
		op_sa(a);
	else if (vb < vc && vc < va)
		op_ra(a);
	else if (vc < va && va < vb)
		op_rra(a);
	else if (va < vc && vc < vb)
	{
		op_rra(a);
		op_sa(a);
	}
	else
	{
		op_ra(a);
		op_sa(a);
	}
}
