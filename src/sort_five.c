/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_five.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:14:51 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:21:53 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	op_rotate_shortest(t_stack *n, int steps)
{
	int	count;

	count = n->size - steps;
	if (steps <= n->size - steps)
	{
		while (steps)
		{
			op_ra(n);
			steps--;
		}
	}
	else
	{
		while (count)
		{
			op_rra(n);
			count--;
		}
	}
}

void	sort_five(t_stack *a, t_stack *b)
{
	int	i;

	i = 0;
	while (i < 2)
	{
		op_rotate_shortest(a, find_min_idx(a));
		op_pb(b, a);
		i++;
	}
	sort_three(a);
	i = 0;
	while (i < 2)
	{
		op_pa(a, b);
		i++;
	}
}
