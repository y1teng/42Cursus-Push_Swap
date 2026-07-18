#include "push_swap.h"

// caluclate
double	disorder_compute(t_stack *a)
{
	int		i;
	int		j;
	double	mistakes;
	double	total_pairs;
	double	ans;

	if (a->size < 2)
		return (0);
	i = 0;
	j = 0;
	mistakes = 0;
	total_pairs = 0;
	while (i < a->size)
	{
		j = i + 1;
		while (j < a->size)
		{
			total_pairs++;
			if (a->data[(a->head + i) % a->capacity]
				> a->data[(a->head + j) % a->capacity])
				mistakes++;
			j++;
		}
		i++;
	}
	ans = mistakes / total_pairs;
	return (ans);
}

// function compute_disorder(stack a):
// mistakes = 0
// total_pairs = 0
// for i from 0 to size(a)-1:
// for j from i+1 to size(a)-1:
// total_pairs += 1
// if a[i] > a[j]:
// mistakes += 1
// return mistakes / total_pairs

// print
void	disorder_print(t_stack *a)
{
	double	ans;

	ans = disorder_compute(a);
	ft_printf("%f\n", ans);
}

// strategy_branch

// int	main(void)
// {
// 	t_stack	a;
// 	int		buffer[2] = {10, 5};

// 	a.capacity = 2;
// 	a.size = 2;
// 	a.data = buffer;
// 	a.head = 0;
// 	disorder_print(&a);
// 	return (0);
// }
