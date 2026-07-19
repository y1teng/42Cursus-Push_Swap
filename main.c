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

int	parse_int(const char *s, int *out)
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
void	stack_init(t_stack *s, int *data, size_t capacity, size_t size)
{
	s->data = data;
	s->capacity = capacity;
	s->size = size;
	s->head = 0;
	s->bench = 0;
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

int	*build_arr(char **argv, int start, int argc, int *out_size)
{
	char	**tokens;
	int		*arr;
	int		size;
	int		i;

	if (argc - start == 1)
	{
		tokens = ft_split(argv[start], ' ');
		size = 0;
		while (tokens[size])
			size++;
		arr = ft_calloc(sizeof(int), size);
		if (parse_atoi_array(arr, tokens, size))
			arr = NULL;
		i = 0;
		while (tokens[i])
			free(tokens[i++]);
		free(tokens);
	}
	else
	{
		size = argc - start;
		arr = ft_calloc(sizeof(int), size);
		if (parse_atoi_array(arr, &argv[start], size))
			arr = NULL;
	}
	*out_size = size;
	return (arr);
}

int	validate_arr(int *arr, int size)
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
t_strategy	resolve_strategy(t_strategy strategy, double disorder)
{
	if (strategy != ADAPTIVE)
		return (strategy);
	else if (disorder < 0.2)
		return (SIMPLE);
	else if (disorder < 0.5)
		return (MEDIUM);
	else
		return (COMPLEX);
}

int	main(int argc, char **argv)
{
	t_options	opt;
	t_stack		a;
	t_stack		b;
	int			*arr;
	int			size;
	double		disorder;
	int			effective;
	int			*counts;
	effective = 0;
	if (argc < 2)
		return (0);
	parse_flags(argc, argv, &opt);
	arr = build_arr(argv, opt.num_start, argc, &size);
	if (!arr || size <= 0 || validate_arr(arr, size))
	{
		free(arr);
		ft_printf("Error\n");
		return (1);
	}
	stack_init(&a, arr, size, size);
	stack_init(&b, ft_calloc(sizeof(int), size), size, 0);
	disorder = disorder_compute(&a);
	effective = resolve_strategy(opt.strategy, disorder);
	counts = ft_calloc(sizeof(int), 11);
	a.counts = counts;
	b.counts = counts;
	if (a.size <= 1 || disorder == 0.0)
		;
	else if (a.size == 2)
	{
		if (a.data[a.head] > a.data[(a.head + 1) % a.capacity])
			op_sa(&a);
	}
	else if (a.size == 3)
		sort_three(&a);
	else if (a.size == 5)
		sort_five(&a, &b);
	else
	{
		if (effective == SIMPLE)
			sort_simple(&a, &b);
		else if (effective == MEDIUM)
			sort_medium(&a, &b);
		else
			sort_complex(&a, &b);
	}
	if (opt.bench)
	{
		static const char *const	strategy_names[] = {
			[ADAPTIVE] = "Adaptive",
			[SIMPLE] = "Simple",
			[MEDIUM] = "Medium",
			[COMPLEX] = "Complex"
		};
		static const char *const	complexity_names[] = {
			[SIMPLE] = "O(n\xc2\xb2)",
			[MEDIUM] = "O(n\xe2\x88\x9an)",
			[COMPLEX] = "O(n log n)"
		};
		int		i;
		int		total_ops;
		int		pct;
		char	*int_str;
		char	frac_buf[3];

		i = 0;
		total_ops = 0;
		while (i < 11)
		{
			total_ops += a.counts[i];
			i++;
		}
		pct = (int)(disorder * 10000 + 0.5);
		int_str = ft_itoa(pct / 100);
		frac_buf[0] = '0' + (pct % 100) / 10;
		frac_buf[1] = '0' + (pct % 100) % 10;
		frac_buf[2] = '\0';
		ft_dprintf(2, "[bench] disorder:  %s.%s%\n", int_str, frac_buf);
		free(int_str);
		ft_dprintf(2, "[bench] strategy:  %s / %s\n",
			strategy_names[opt.strategy], complexity_names[effective]);
		ft_dprintf(2, "[bench] total_ops: %d\n", total_ops);
		ft_dprintf(2, "[bench] sa: %d  sb: %d  ss: %d  pa: %d  pb: %d\n",
			a.counts[OP_SA], a.counts[OP_SB], a.counts[OP_SS],
			a.counts[OP_PA], a.counts[OP_PB]);
		ft_dprintf(2, "[bench] ra: %d  rb: %d  rr: %d  rra: %d  rrb: %d  rrr: %d\n",
			a.counts[OP_RA], a.counts[OP_RB], a.counts[OP_RR],
			a.counts[OP_RRA], a.counts[OP_RRB], a.counts[OP_RRR]);
	}
	free(a.data);
	free(b.data);
	free(counts);
	return (0);
}
