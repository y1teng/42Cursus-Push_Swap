#ifndef PUSH_SWAP_H
# define PUSH_SWAP_H

# include "libft/libft.h"
# include "libftprintf/ft_printf.h"
# include <limits.h>
# include <stdbool.h>
# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
typedef enum e_strategy
{
	ADAPTIVE,
	SIMPLE,
	MEDIUM,
	COMPLEX
}				t_strategy;

typedef enum e_op
{
	OP_SA,
	OP_SB,
	OP_SS,
	OP_PA,
	OP_PB,
	OP_RA,
	OP_RB,
	OP_RR,
	OP_RRA,
	OP_RRB,
	OP_RRR
}				t_op;

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

void			sort_simple(t_stack *a, t_stack *b);

//add complex
void sort_complex(t_stack *a, t_stack *b);

// add 3 sort
void			sort_three(t_stack *a);
void			sort_five(t_stack *a, t_stack *b);
void			op_rotate_shortest(t_stack *n, int steps);
void			op_push(t_stack *dst, t_stack *src);
void			op_pa(t_stack *dst, t_stack *src);
void			op_pb(t_stack *dst, t_stack *src);

void			op_rotate(t_stack *n);
void			op_ra(t_stack *a);
void			op_rb(t_stack *b);
void			op_rr(t_stack *a, t_stack *b);

void	get_min_max(t_stack *a, int *min, int *max);
void			op_reverse_rotate(t_stack *n);
void			op_rra(t_stack *a);
void			op_rrb(t_stack *b);
void			op_rrr(t_stack *a, t_stack *b);

void			op_swap(t_stack *n);
void			op_sa(t_stack *a);
void			op_sb(t_stack *b);
void			op_ss(t_stack *a, t_stack *b);

void			print_op(t_op op);

#endif
