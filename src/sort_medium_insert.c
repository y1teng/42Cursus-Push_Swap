/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium_insert.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 22:45:29 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:45:29 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	count_smaller(t_stack *a, int val)
{
	int	count;
	int	small_cnt;

	small_cnt = 0;
	count = 0;
	while (count < a->size)
	{
		if (stack_at(a, count) < val)
			small_cnt++;
		count++;
	}
	return (small_cnt);
}

int	find_min_idx(t_stack *a)
{
	int	count;
	int	min;
	int	min_idx;

	min = stack_at(a, 0);
	min_idx = 0;
	count = 0;
	while (count < a->size)
	{
		if (stack_at(a, count) < min)
		{
			min = stack_at(a, count);
			min_idx = count;
		}
		count++;
	}
	return (min_idx);
}

void	insert_back(t_stack *a, t_stack *b)
{
	int	b_val;

	while (b->size > 0)
	{
		b_val = b->data[b->head];
		op_rotate_shortest(a, count_smaller(a, b_val));
		op_pa(a, b);
		op_rotate_shortest(a, find_min_idx(a));
	}
}
