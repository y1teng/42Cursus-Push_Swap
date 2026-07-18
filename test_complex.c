#include "push_swap.h"

static void	stack_init(t_stack *s, int *data, int capacity, int size)
{
	s->data = data;
	s->capacity = capacity;
	s->size = size;
	s->head = 0;
}

int	main(int argc, char **argv)
{
	t_stack	a;
	t_stack	b;
	int		*arr;
	int		i;
	int		size;

	size = argc - 1;
	if (size < 2)
		return (0);
	arr = ft_calloc(sizeof(int), size);
	i = 0;
	while (i < size)
	{
		arr[i] = ft_atoi(argv[i + 1]);
		i++;
	}
	stack_init(&a, arr, size, size);
	stack_init(&b, ft_calloc(sizeof(int), size), size, 0);
	sort_complex(&a, &b);
	free(a.data);
	free(b.data);
	return (0);
}
