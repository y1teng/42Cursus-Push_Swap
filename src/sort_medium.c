/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:14:55 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 21:45:48 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	isqrt(int n)
{
	int	i;

	if (n == 0)
		return (0);
	i = 0;
	while ((i + 1) * (i + 1) <= n)
		i++;
	return (i);
}

static int	get_chunk(int value, int min, int max, int num_chunks)
{
	int	nm_c;

	nm_c = ((value - min) * num_chunks) / (max - min + 1);
	return (nm_c);
}
static void	push_chunk(t_stack *a, t_stack *b, int chunk_idx, int min, int max,
		int num_chunks)
{
	int	found;
	int	found_idx;
	int	count;

	found = 1;
	found_idx = -1;
	count = 0;
	if (!a || !b)
		return ;
	while (found)
	{
		found = 0;
		count = 0;
		while (count < a->size)
		{
			if (get_chunk(a->data[(a->head + count) % a->capacity], min, max,
					num_chunks) == chunk_idx)
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

static void	get_min_max(t_stack *a, int *min, int *max)
{
	int	min_val;
	int	max_val;
	int	count;

	if (!a)
		return ;
	count = 0;
	min_val = a->data[(a->head + count) % a->capacity];
	max_val = a->data[(a->head + count) % a->capacity];
	count = 0;
	while (count < a->size)
	{
		if (a->data[(a->head + count) % a->capacity] < min_val)
			min_val = a->data[(a->head + count) % a->capacity];
		else if (a->data[(a->head + count) % a->capacity] > max_val)
			max_val = a->data[(a->head + count) % a->capacity];
		count++;
	}
	*min = min_val;
	*max = max_val;
}

static void	distribute_chunks(t_stack *a, t_stack *b, int num_chunks)
{
	int	min_val;
	int	max_val;
	int	chunk_idx;

	chunk_idx = 0;
	get_min_max(a, &min_val, &max_val);
	while (chunk_idx < num_chunks)
	{
		push_chunk(a, b, chunk_idx, min_val, max_val, num_chunks);
		chunk_idx++;
	}
}

static void	insert_back(t_stack *a, t_stack *b)
{
	int	min;
	int	count;
	int	min_idx;
	int	b_val;
	int	small_cnt;

	small_cnt = 0;
	while (b->size > 0)
	{
		b_val = b->data[b->head];
		small_cnt = 0;
		count = 0;
		while (count < a->size)
		{
			if (a->data[(a->head + count) % a->capacity] < b_val)
				++small_cnt;
			count++;
		}
		op_rotate_shortest(a, small_cnt);
		op_pa(a, b);
		min_idx = 0;
		count = 0;
		min = a->data[(a->head + count) % a->capacity];
		while (count < a->size)
		{
			if (a->data[(a->head + count) % a->capacity] < min)
			{
				min = a->data[(a->head + count) % a->capacity];
				min_idx = count;
			}
			count++;
		}
		op_rotate_shortest(a, min_idx);
	}
}

void	sort_medium(t_stack *a, t_stack *b)
{
	int	num_chunks;

	num_chunks = isqrt(a->size);
	if (num_chunks < 1)
		num_chunks = 1;
	distribute_chunks(a, b, num_chunks);
	insert_back(a, b);
}
