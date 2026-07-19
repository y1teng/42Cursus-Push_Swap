/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sort_medium.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:14:55 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:06:10 by ayaito           ###   ########.fr       */
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

void	sort_medium(t_stack *a, t_stack *b)
{
	int	num_chunks;

	num_chunks = isqrt(a->size);
	if (num_chunks < 1)
		num_chunks = 1;
	distribute_chunks(a, b, num_chunks);
	insert_back(a, b);
}
