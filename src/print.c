/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:13:37 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:30:26 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static const char	*strategy_name(t_strategy strategy)
{
	static const char *const	names[] = {[ADAPTIVE] = "Adaptive",
	[SIMPLE] = "Simple", [MEDIUM] = "Medium", [COMPLEX] = "Complex"};

	return (names[strategy]);
}

static const char	*complexity_name(t_strategy strategy)
{
	static const char *const	names[] = {[SIMPLE] = "O(n\xc2\xb2)",
	[MEDIUM] = "O(n\xe2\x88\x9an)", [COMPLEX] = "O(n log n)"};

	return (names[strategy]);
}

static void	print_disorder(double disorder)
{
	int		pct;
	char	*int_str;
	char	frac_buf[3];

	pct = (int)(disorder * 10000 + 0.5);
	int_str = ft_itoa(pct / 100);
	frac_buf[0] = '0' + (pct % 100) / 10;
	frac_buf[1] = '0' + (pct % 100) % 10;
	frac_buf[2] = '\0';
	ft_dprintf(2, "[bench] disorder:  %s.%s%\n", int_str, frac_buf);
	free(int_str);
}

void	print_bench(t_options opt, t_stack a, double disorder,
		t_strategy effective)
{
	int	i;
	int	total_ops;

	i = 0;
	total_ops = 0;
	while (i < 11)
	{
		total_ops += a.counts[i];
		i++;
	}
	print_disorder(disorder);
	ft_dprintf(2, "[bench] strategy:  %s / %s\n", strategy_name(opt.strategy),
		complexity_name(effective));
	ft_dprintf(2, "[bench] total_ops: %d\n", total_ops);
	ft_dprintf(2, "[bench] sa: %d  sb: %d  ss: %d  pa: %d  pb: %d\n",
		a.counts[OP_SA], a.counts[OP_SB], a.counts[OP_SS], a.counts[OP_PA],
		a.counts[OP_PB]);
	ft_dprintf(2, "[bench] ra: %d  rb: %d  rr: %d  rra: %d  rrb: %d  rrr: %d\n",
		a.counts[OP_RA], a.counts[OP_RB], a.counts[OP_RR], a.counts[OP_RRA],
		a.counts[OP_RRB], a.counts[OP_RRR]);
}

void	print_op(t_stack *n, t_op op)
{
	static const char *const	names[] = {[OP_SA] = "sa", [OP_SB] = "sb",
	[OP_SS] = "ss", [OP_PA] = "pa", [OP_PB] = "pb", [OP_RA] = "ra",
	[OP_RB] = "rb", [OP_RR] = "rr", [OP_RRA] = "rra", [OP_RRB] = "rrb",
	[OP_RRR] = "rrr"};

	if (!n->bench)
		ft_printf("%s\n", names[op]);
	n->counts[op]++;
}
