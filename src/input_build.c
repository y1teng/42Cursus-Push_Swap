/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   input_build.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 22:45:29 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:45:29 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	*build_arr_from_string(char *str, int *out_size)
{
	char	**tokens;
	int		*arr;
	int		size;
	int		i;

	tokens = ft_split(str, ' ');
	size = 0;
	while (tokens[size])
		size++;
	arr = ft_calloc(sizeof(int), size);
	if (parse_atoi_array(arr, tokens, size))
	{
		free(arr);
		arr = NULL;
	}
	i = 0;
	while (tokens[i])
		free(tokens[i++]);
	free(tokens);
	*out_size = size;
	return (arr);
}

int	*build_arr(char **argv, int start, int argc, int *out_size)
{
	int	*arr;
	int	size;

	if (argc - start == 1)
		return (build_arr_from_string(argv[start], out_size));
	size = argc - start;
	arr = ft_calloc(sizeof(int), size);
	if (parse_atoi_array(arr, &argv[start], size))
	{
		free(arr);
		arr = NULL;
	}
	*out_size = size;
	return (arr);
}
