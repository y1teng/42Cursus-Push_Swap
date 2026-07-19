/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_parsing.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 22:45:29 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:45:29 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

void	parse_flags(int argc, char **argv, t_options *opt)
{
	int	i;

	opt->strategy = ADAPTIVE;
	opt->bench = 0;
	i = 1;
	while (i < argc)
	{
		if (ft_strncmp(argv[i], "--adaptive", 11) == 0)
			opt->strategy = ADAPTIVE;
		else if (ft_strncmp(argv[i], "--simple", 9) == 0)
			opt->strategy = SIMPLE;
		else if (ft_strncmp(argv[i], "--medium", 9) == 0)
			opt->strategy = MEDIUM;
		else if (ft_strncmp(argv[i], "--complex", 10) == 0)
			opt->strategy = COMPLEX;
		else if (ft_strncmp(argv[i], "--bench", 8) == 0)
			opt->bench = 1;
		else
			break ;
		i++;
	}
	opt->num_start = i;
}

static int	parse_sign(const char **s)
{
	int	sign;

	sign = 1;
	if (**s == '-' || **s == '+')
	{
		if (**s == '-')
			sign = -1;
		(*s)++;
	}
	return (sign);
}

int	parse_int(const char *s, int *out)
{
	long	n;
	int		sign;

	sign = parse_sign(&s);
	n = 0;
	if (!*s)
		return (1);
	while (*s)
	{
		if (*s < '0' || '9' < *s)
			return (1);
		n = n * 10 + (*s - '0');
		if (sign == 1 && n > 2147483647)
			return (1);
		if (sign == -1 && n > 2147483648)
			return (1);
		s++;
	}
	*out = (int)(n * sign);
	return (0);
}

int	parse_atoi_array(int *arr, char **argv, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		if (parse_int(argv[i], &arr[i]))
			return (1);
		i++;
	}
	return (0);
}
