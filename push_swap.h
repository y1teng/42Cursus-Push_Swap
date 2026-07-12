#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

#include "libft/libft.h"
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum e_strategy
{
	ADAPTIVE,
	SIMPLE,
	MEDIUM,
	COMPLEX
}				t_strategy;

typedef struct s_options
{
	t_strategy	strategy;
	int			bench;
	int			num_start;
}				t_options;

typedef struct s_stack
{
	int			*data;
	int			capacity;
	int			size;
	int			head;
}				t_stack;


void sort_simple(t_stack *a,t_stack *b);

// operations
void			op_push(t_stack *dst, t_stack *src);
void			op_pa(t_stack *dst, t_stack *src);
void			op_pb(t_stack *dst, t_stack *src);

void			op_rotate(t_stack *n);
void			op_ra(t_stack *a);
void			op_rb(t_stack *b);
void			op_rr(t_stack *a, t_stack *b);

void			op_reverse_rotate(t_stack *n);
void			op_rra(t_stack *a);
void			op_rrb(t_stack *b);
void			op_rrr(t_stack *a, t_stack *b);

void			op_swap(t_stack *n);
void			op_sa(t_stack *a);
void			op_sb(t_stack *b);
void			op_ss(t_stack *a, t_stack *b);

#endif
