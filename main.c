#include "push_swap.h"

void	parse_flags(int argc, char **argv, t_options *opt){
	apt->strategy = ADAPTIVE;
	opt->bench = 0;
	int i;
	i = 1;

	if(ft_strncmp(argv[i],"--simple",9) == 0)
		opt->strategy = SIMPLE;
}

static int	parse_has_overlap(int *arr, int size)
{
	int	i;
	int	j;

	i = 0;
	while (i < size)
	{
		j = i + 1;
		while (j < size)
		{
			if (arr[i] == arr[j])
				return (1);
			j++;
		}
		i++;
	}
	return (0);
}

static int	parse_int(const char *s, int *out)
{
	long	n;
	int		sign;

	n = 0;
	sign = 1;
	if (*s == '-' || *s == '+')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
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

static int	parse_atoi_array(int *arr, char **argv, int size)
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
#include <stdio.h>

int	main(int argc, char **argv)
{
	int	arr[100];
	int	i;
	int	size;

	if (argc < 2)
		return (0);
	if(argc == 2 && is_space())
	size = argc - 1;
	if (parse_atoi_array(arr, &argv[1], size) || parse_has_overlap(arr, size))
	{
		printf("Error\n");
		return (1);
	}
	i = 0;
	while (i < size)
	{
		printf("%d\n", arr[i]);
		i++;
	}
	return (0);
}
