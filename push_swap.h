#ifndef PUSH_SWAP_H
#define PUSH_SWAP_H

typedef struct s_stack
{
	int *data;
	int capacity;
	int size;
	int head;
} t_stack;


int	find_min_index(t_stack *a);

#endif
