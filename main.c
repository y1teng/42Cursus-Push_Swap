#include "push_swap.h"

void	parse_flags(int argc, char **argv, t_options *opt){
	opt->strategy = ADAPTIVE;
	opt->bench = 0;
	int i;
	i = 1;
	
	while(i < argc)
	{
		if(ft_strncmp(argv[i],"--adaptive",11) == 0)
			opt->strategy = ADAPTIVE;
		else if(ft_strncmp(argv[i],"--simple",9) == 0)
			opt->strategy = SIMPLE;
		else if(ft_strncmp(argv[i],"--medium",9) == 0)
			opt->strategy = MEDIUM;
		else if(ft_strncmp(argv[i],"--complex",10) == 0)
			opt->strategy = COMPLEX;
		else if(ft_strncmp(argv[i],"--bench",8) == 0)
			opt->bench = 1;
		else
			break;
		i++;
	}
	opt->num_start = i;


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
	int	size;
	t_options opt;
	t_stack a;
	t_stack b;

	if (argc < 2)
		return (0);
	parse_flags(argc, argv, &opt);
	size = argc - opt.num_start;
	if(size <= 0)
		return 0;
	if (parse_atoi_array(arr, &argv[opt.num_start], size) || parse_has_overlap(arr, size))
	{
		printf("Error\n");
		return (1);
	}
	a.data = arr;
	a.capacity = size;
	a.size = size;
	a.head = 0;
	int buffer_b[100];
	b.data = buffer_b;
	b.capacity = size;
	b.size = 0;
	b.head = 0;
	sort_simple(&a,&b);

	return (0);
}
