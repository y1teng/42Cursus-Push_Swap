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
// 「通常引数」or「単一スペース区切り」を吸収して、
// arr(malloc済み)とsizeを確定させて返す。ここが今回の分岐の全責任を持つ

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

// parse_int相当のエラーチェック + parse_has_overlap
// ※実は parse_int は build_arr の中で数値変換する時に使うはず
//   なので validate は「重複チェックのみ」でもいい

int	main(int argc, char **argv)
{
	t_options	opt;
	t_stack		a;
	t_stack		b;
	int			*arr;
	int			size;

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
	
	if (a.size <= 1)
		; // 1個以下は既にソート済み、何もしない
	else if (a.size == 2)
	{
		if (a.data[a.head] > a.data[(a.head + 1) % a.capacity])
			op_sa(&a);
	}
	else if (a.size == 3)
		sort_three(&a);
	else if (a.size == 5)
		sort_five(&a,&b);
	else
		sort_simple(&a, &b); // 4個以上はまだ選択ソートに任せる(今後ここを拡張)
	free(a.data);
	free(b.data);
	return (0);
}
