/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:11:20 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:34:20 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	stack_init(t_stack *s, int *data, size_t capa, size_t size)
{
	s->data = data;
	s->capa = capa;
	s->size = size;
	s->head = 0;
	s->bench = 0;
}

int	parse_and_build(int argc, char **argv, t_options *opt, t_stack *a)
{
	int	*arr;
	int	size;

	parse_flags(argc, argv, opt);
	arr = build_arr(argv, opt->num_start, argc, &size);
	if (!arr || size <= 0 || validate_arr(arr, size))
	{
		free(arr);
		ft_dprintf(2, "Error\n");
		return (1);
	}
	stack_init(a, arr, size, size);
	return (0);
}

static void	dispatch_sort(t_stack *a, t_stack *b, t_strategy effective,
		double disorder)
{
	if (a->size <= 1 || disorder == 0.0)
		;
	else if (a->size == 2)
	{
		if (a->data[a->head] > a->data[(a->head + 1) % a->capa])
			op_sa(a);
	}
	else if (a->size == 3)
		sort_three(a);
	else if (a->size == 5)
		sort_five(a, b);
	else if (effective == SIMPLE)
		sort_simple(a, b);
	else if (effective == MEDIUM)
		sort_medium(a, b);
	else
		sort_complex(a, b);
}

int	main(int argc, char **argv)
{
	t_options	opt;
	t_stack		a;
	t_stack		b;
	double		disorder;
	int			effective;

	if (argc < 2)
		return (0);
	if (parse_and_build(argc, argv, &opt, &a))
		return (1);
	stack_init(&b, ft_calloc(sizeof(int), a.size), a.size, 0);
	a.counts = ft_calloc(sizeof(int), 11);
	b.counts = a.counts;
	disorder = disorder_compute(&a);
	effective = resolve_strategy(opt.strategy, a.size, disorder);
	dispatch_sort(&a, &b, effective, disorder);
	if (opt.bench)
		print_bench(opt, a, disorder, effective);
	free(a.data);
	free(b.data);
	free(a.counts);
	return (0);
}
