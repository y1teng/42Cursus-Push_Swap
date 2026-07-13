#include "push_swap.h"

/**
 * @brief argv[1] 以降を先頭から見て `--` で始まる間だけフラグとして読む。
 *
 * `--simple --bench` のように複数個連続すること想定。`--` で始まらない
 * 引数が出た時点で止まり、そのインデックスを opt->num_start に記録する
 * （そこから先が実際にソートする整数列）。
 */
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

/** @brief arr の中に同じ値が2つ以上あれば1を返す（重複チェック）。 */
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

/** @brief argv[0..size) を順に parse_int() して arr に書く。失敗したら1。 */
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

/**
 * @brief エントリーポイント。フラグと整数列をパースし、a/b の t_stack を
 * 組み立てて sort_simple() に渡す。
 *
 * `capacity` はどちらのスタックも入力個数ぶん確保する。b は最初 size=0
 * （空）で、a からの pb でしか値が入らない。
 */
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
	sort_simple(&a, &b);
	free(a.data);
	free(b.data);
	return (0);
}
