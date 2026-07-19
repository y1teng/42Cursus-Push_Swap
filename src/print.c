/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:13:37 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 19:17:53 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	print_op(t_stack *n, t_op op)
{
	static const char *const names[] = {[OP_SA] = "sa", [OP_SB] = "sb",
		[OP_SS] = "ss", [OP_PA] = "pa", [OP_PB] = "pb", [OP_RA] = "ra",
		[OP_RB] = "rb", [OP_RR] = "rr", [OP_RRA] = "rra", [OP_RRB] = "rrb",
		[OP_RRR] = "rrr"};
	if (!n->bench)
		ft_printf("%s\n", names[op]);
	n->counts[op]++;
}
