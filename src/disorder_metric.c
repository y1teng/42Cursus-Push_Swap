/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   disorder_metric.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:04:34 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:20:17 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static double	count_mismatches(t_stack *a)
{
	int		i;
	int		j;
	double	mistakes;

	i = 0;
	j = 0;
	mistakes = 0;
	while (i < a->size)
	{
		j = i + 1;
		while (j < a->size)
		{
			if (a->data[(a->head + i) % a->capa] > a->data[(a->head + j)
					% a->capa])
				mistakes++;
			j++;
		}
		i++;
	}
	return (mistakes);
}

double	disorder_compute(t_stack *a)
{
	double	total_pairs;

	if (a->size < 2)
		return (0);
	total_pairs = (double)a->size * (a->size - 1) / 2;
	return (count_mismatches(a) / total_pairs);
}
