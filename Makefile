NAME = push_swap

CC = cc

CFLAGS = -Wall -Wextra -Werror

SRCS =	main.c	\
		op_p.c	\
		op_r.c	\
		op_rr.c	\
		op_s.c	\
		print.c	\
		sort_simple.c	\
		sort_medium.c \
		sort_complex.c \
		sort_three.c	\
		sort_five.c	\
		disorder_metric.c	\
		push_swap.c

HEADER = push_swap.h

OBJS = $(SRCS:.c=.o)

LIBFT = libft/libft.a

LIBFTPRINTF = libftprintf/libftprintf.a

all: $(NAME)

$(NAME): $(OBJS)
	$(MAKE) -C libft
	$(MAKE) -C libftprintf
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LIBFTPRINTF) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	$(MAKE) -C libft clean
	$(MAKE) -C libftprintf clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C libft fclean
	$(MAKE) -C libftprintf fclean

re: fclean all

.PHONY : all clean fclean re
