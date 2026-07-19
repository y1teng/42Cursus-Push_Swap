# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/19 19:11:28 by ayaito            #+#    #+#              #
#    Updated: 2026/07/19 21:39:29 by ayaito           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = push_swap

CC = cc

CFLAGS = -Wall -Wextra -Werror -I$(HEADER_PATH) -I.

SRC_PATH = src/
OBJ_PATH = obj/
HEADER_PATH = include/

SRC =	main.c	\
		op_p.c	\
		op_r.c	\
		op_rr.c	\
		op_s.c	\
		ft_dprintf.c \
		print.c	\
		sort_simple.c	\
		sort_medium.c \
		sort_complex.c \
		sort_three.c	\
		sort_five.c	\
		disorder_metric.c

SRCS = $(addprefix $(SRC_PATH), $(SRC))

HEADER = push_swap.h
HEADERS = $(addprefix $(HEADER_PATH), $(HEADER))

OBJ = $(SRC:.c=.o)
OBJS = $(addprefix $(OBJ_PATH), $(OBJ))

LIBFT = libft/libft.a
LIBFTPRINTF = libftprintf/libftprintf.a

all: $(NAME)

$(NAME): $(OBJS)
	$(MAKE) -C libft
	$(MAKE) -C libftprintf
	$(CC) $(CFLAGS) $(OBJS) $(LIBFT) $(LIBFTPRINTF) -o $(NAME)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c $(HEADERS) | $(OBJ_PATH)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_PATH):
	mkdir -p $(OBJ_PATH)

clean:
	rm -f $(OBJ)
	$(MAKE) -C libft clean
	$(MAKE) -C libftprintf clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C libft fclean
	$(MAKE) -C libftprintf fclean

re: fclean all

.PHONY : all clean fclean re
