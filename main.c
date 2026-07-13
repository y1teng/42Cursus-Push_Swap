#include "push_swap.h"

/**
 * @brief argv[1] 以降を先頭から見て `--` で始まる間だけフラグとして読む。
 *
 * `--simple --bench` のように複数個連続すること想定。`--` で始まらない
 * 引数が出た時点で止まり、そのインデックスを opt->num_start に記録する
 * （そこから先が実際にソートする整数列）。
 */
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

/** @brief arr の中に同じ値が2つ以上あれば1を返す（重複チェック）。 */
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

/**
 * @brief 文字列 s を int に変換する。
 * @return 数字以外の文字を含む、空文字、または int の範囲を超える場合は 1。
 * 成功時は 0 を返し、変換結果を *out に書く。
 */
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
#include <stdio.h>

/**
 * @brief エントリーポイント。フラグと整数列をパースし、a/b の t_stack を
 * 組み立てて sort_simple() に渡す。
 *
 * `capacity` はどちらのスタックも入力個数ぶん確保する。b は最初 size=0
 * （空）で、a からの pb でしか値が入らない。
 */
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
