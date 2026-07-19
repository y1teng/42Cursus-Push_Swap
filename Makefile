# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/07/19 19:11:28 by ayaito            #+#    #+#              #
#    Updated: 2026/07/19 23:21:33 by ayaito           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = push_swap

CC = cc

CFLAGS = -Wall -Wextra -Werror -I$(HEADER_PATH) -I.

SRC_PATH = src/
OBJ_PATH = obj/
HEADER_PATH = include/

SRC =	disorder_metric.c \
		input_build.c \
		input_parsing.c \
		input_strategy.c \
		input_validate.c \
		main.c \
		op_p.c \
		op_r.c \
		op_rr.c \
		op_s.c \
		print.c \
		print_ft_dprintf.c \
		sort_complex.c \
		sort_five.c \
		sort_medium.c \
		sort_medium_chunk.c \
		sort_medium_insert.c \
		sort_simple.c \
		sort_three.c

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
	rm -rf $(OBJ_PATH)
	$(MAKE) -C libft clean
	$(MAKE) -C libftprintf clean

fclean: clean
	rm -f $(NAME)
	$(MAKE) -C libft fclean
	$(MAKE) -C libftprintf fclean

re: fclean all

.PHONY : all clean fclean re
