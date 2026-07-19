/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   strategy.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 22:26:53 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:26:55 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

t_strategy	resolve_strategy(t_strategy strategy, int size, double disorder)
{
	if (strategy != ADAPTIVE)
		return (strategy);
	if (strategy == ADAPTIVE && (size == 2 || size == 3 || size == 5))
		return (SIMPLE);
	else if (disorder < 0.2)
		return (SIMPLE);
	else if (disorder < 0.5)
		return (MEDIUM);
	else
		return (COMPLEX);
}
