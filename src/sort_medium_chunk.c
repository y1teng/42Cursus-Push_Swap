/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium_chunk.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 22:45:29 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:45:29 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

int	stack_at(t_stack *a, int idx)
{
	return (a->data[(a->head + idx) % a->capa]);
}

static int	get_chunk(int value, t_chunk_range range)
{
	return (((value - range.min) * range.num_chunks)
		/ (range.max - range.min + 1));
}

static void	get_min_max(t_stack *a, int *min, int *max)
{
	int	count;
	int	val;

	*min = stack_at(a, 0);
	*max = stack_at(a, 0);
	count = 0;
	while (count < a->size)
	{
		val = stack_at(a, count);
		if (val < *min)
			*min = val;
		else if (val > *max)
			*max = val;
		count++;
	}
}

static void	push_chunk(t_stack *a, t_stack *b, int chunk_idx,
		t_chunk_range range)
{
	int	found;
	int	found_idx;
	int	count;

	found = 1;
	while (found)
	{
		found = 0;
		count = 0;
		while (count < a->size)
		{
			if (get_chunk(stack_at(a, count), range) == chunk_idx)
			{
				found = 1;
				found_idx = count;
				break ;
			}
			count++;
		}
		if (found)
		{
			op_rotate_shortest(a, found_idx);
			op_pb(b, a);
		}
	}
}

void	distribute_chunks(t_stack *a, t_stack *b, int num_chunks)
{
	t_chunk_range	range;
	int				chunk_idx;

	range.num_chunks = num_chunks;
	get_min_max(a, &range.min, &range.max);
	chunk_idx = 0;
	while (chunk_idx < num_chunks)
	{
		push_chunk(a, b, chunk_idx, range);
		chunk_idx++;
	}
}
