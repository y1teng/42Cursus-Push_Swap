/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_complex.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:14:41 by ykomori           #+#    #+#             */
/*   Updated: 2026/07/19 21:45:48 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static void	sort_array(int *arr, int size)
{
	int	i;
	int	j;
	int	tmp;

	i = 0;
	while (i < size - 1)
	{
		j = 0;
		while (j < size - 1 - i)
		{
			if (arr[j] > arr[j + 1])
			{
				tmp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = tmp;
			}
			j++;
		}
		i++;
	}
}

static int	find_rank(int *sorted, int size, int value)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (sorted[i] == value)
			return (i);
		i++;
	}
	return (-1);
}

void	ranks_convert(int *arr, int size)
{
	int	*sorted;
	int	i;

	sorted = ft_calloc(sizeof(int), size);
	if (!sorted)
		return ;
	i = 0;
	while (i < size)
	{
		sorted[i] = arr[i];
		i++;
	}
	sort_array(sorted, size);
	i = 0;
	while (i < size)
	{
		arr[i] = find_rank(sorted, size, arr[i]);
		i++;
	}
	free(sorted);
}

static int	count_bits(int size)
{
	int	bits;
	int	max_rank;

	bits = 0;
	max_rank = size - 1;
	while ((max_rank >> bits) > 0)
		bits++;
	return (bits);
}

void	sort_complex(t_stack *a, t_stack *b)
{
	int	bits;
	int	i;
	int	j;
	int	n;

	ranks_convert(a->data, a->size);
	bits = count_bits(a->size);
	i = 0;
	while (i < bits)
	{
		n = a->size;
		j = 0;
		while (j < n)
		{
			if (((a->data[a->head] >> i) & 1) == 0)
				op_pb(b, a);
			else
				op_ra(a);
			j++;
		}
		while (b->size > 0)
			op_pa(a, b);
		i++;
	}
}
