#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

typedef struct s_stack
{
	int		nm;
	struct s_stack	*next;
}			t_stack;

typedef struct s_stack_d
{
	int		nm;
	struct s_stack_d* prev;
	struct s_stack_d* next;
}			t_stack_d;

#endif
