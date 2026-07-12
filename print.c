
#include "push_swap.h"

void	print_op(t_op op)
{
	static const char *const	names[] = {
		[OP_SA] = "sa", [OP_SB] = "sb", [OP_SS] = "ss",
		[OP_PA] = "pa", [OP_PB] = "pb",
		[OP_RA] = "ra", [OP_RB] = "rb", [OP_RR] = "rr",
		[OP_RRA] = "rra", [OP_RRB] = "rrb", [OP_RRR] = "rrr"
	};
	ft_printf("%s\n",names[op]);
}
