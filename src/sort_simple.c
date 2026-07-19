/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_simple.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:11:09 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 19:11:13 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	sort_find_min_index(t_stack *a)
{
	int	count;
	int	i;
	int	min;
	int	min_idx;

	count = 0;
	min_idx = 0;
	i = a->head;
	min = a->data[i];
	if (!a)
		return (-1);
	while (count < a->size)
	{
		i = (a->head + count) % a->capacity;
		if (min > a->data[i])
		{
			min = a->data[i];
			min_idx = count;
		}
		count++;
	}
	return (min_idx);
}

void	sort_simple(t_stack *a, t_stack *b)
{
	int	i;
	int	steps;
	int	origin_size;

	origin_size = a->size;
	i = 0;
	while (i < origin_size)
	{
		steps = sort_find_min_index(a);
		while (steps)
		{
			op_ra(a);
			steps--;
		}
		op_pb(b, a);
		i++;
	}
	i = 0;
	while (i < origin_size)
	{
		op_pa(a, b);
		i++;
	}
}
