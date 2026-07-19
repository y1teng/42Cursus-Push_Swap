/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   push_swap.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:14:24 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 22:32:19 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	int			capa;
	int			size;
	int			head;
	int			*counts;
	int			bench;
}				t_stack;

typedef struct s_chunk_range
{
	int	min;
	int	max;
	int	num_chunks;
}				t_chunk_range;

void			sort_simple(t_stack *a, t_stack *b);
void			sort_medium(t_stack *a, t_stack *b);
int				stack_at(t_stack *a, int idx);
void			distribute_chunks(t_stack *a, t_stack *b, int num_chunks);
void			insert_back(t_stack *a, t_stack *b);
int				find_min_idx(t_stack *a);
void			sort_complex(t_stack *a, t_stack *b);
void			sort_three(t_stack *a);
void			sort_five(t_stack *a, t_stack *b);
double			disorder_compute(t_stack *a);

// input
void			parse_flags(int argc, char **argv, t_options *opt);
int				parse_int(const char *s, int *out);
int				parse_atoi_array(int *arr, char **argv, int size);
int				*build_arr(char **argv, int start, int argc, int *out_size);
int				validate_arr(int *arr, int size);
t_strategy		resolve_strategy(t_strategy strategy, int size,
					double disorder);
int				parse_and_build(int argc, char **argv, t_options *opt,
					t_stack *a);

// operation
void			op_rotate_shortest(t_stack *n, int steps);
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

// print
void			print_op(t_stack *n, t_op op);
int				ft_dprintf(int fd, const char *fmt, ...);
void			print_bench(t_options opt, t_stack a, double disorder,
					t_strategy effective);

#endif
