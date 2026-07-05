#include "push_swap.h"
#include <stdio.h>
#include <stdlib.h>

void	op_sa(t_stack **lst)
{
	struct s_stack	*tmp;

	if (!lst)
		return ;
	tmp = 
}

t_stack	*ft_lstnew(int content)
{
	t_stack	*new_node;

	new_node = (t_stack *)calloc(sizeof(t_stack), 1);
	if (!new_node)
		return (NULL);
	new_node->nm = content;
	new_node->next = NULL;
	return (new_node);
}

void	ft_lstadd_front(t_stack **lst, t_stack *new)
{
	if (!*lst)
	{
		*lst = new;
		return ;
	}
	new->next = *lst;
	*lst = new;
}

void	ft_lstadd_back(t_stack **lst, t_stack *new)
{
	t_stack	*last;

	if (!lst || !new)
		return ;
	if (!*lst)
	{
		ft_lstadd_front(lst, new);
		return ;
	}
	last = *lst;
	while (last->next)
		last = last->next;
	last->next = new;
}

int	main(void)
{
	t_stack	**new_node;
	int		i;
	int		input[5] = {1, 4, 5, 2, 8};

	i = 0;
	new_node = (t_stack **)malloc(sizeof(t_stack *) * 5);
	
	while (i < 5)
	{
		new_node[i] = ft_lstnew(input[i]);
		ft_lstadd_back(, );
		i++;
	}
	i = 0;
	while (i < 5)
	{
		printf("%p->%p\n", new_node[i], new_node[i]->next);
		i++;
	}
	i = 0;
	op_sa(new_node);
	printf("\n");
	while (i < 5)
	{
		printf("%p->%p\n", new_node[i], new_node[i]->next);
		i++;
	}
	return (0);
}
