/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_dprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayaito <ayaito@student.42tokyo.jp>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 19:04:43 by ayaito            #+#    #+#             */
/*   Updated: 2026/07/19 19:19:04 by ayaito           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "push_swap.h"

static int	ft_dprintf_putstr_fd(char *s, int fd)
{
	return (write(fd, s, ft_strlen(s)));
}
static int	ft_dprintf_putnbr_fd(int n, int fd)
{
	char			buf[11];
	char			*tmp;
	unsigned int	u_n;

	tmp = &buf[11];
	if (n < 0)
		u_n = -(unsigned int)n;
	else
		u_n = (unsigned int)n;
	while (1)
	{
		*--tmp = u_n % 10 + '0';
		u_n /= 10;
		if (!u_n)
			break ;
	}
	if (n < 0)
		*--tmp = '-';
	return (write(fd, tmp, &buf[11] - tmp));
}

int	ft_dprintf(int fd, const char *fmt, ...)
{
	int		ans;
	va_list	ap;

	ans = 0;
	if (!fmt)
		return (-1);
	va_start(ap, fmt);
	while (*fmt)
	{
		if (*fmt == '%' && (*(fmt + 1) == 's' || *(fmt + 1) == 'd'))
		{
			if (*(fmt + 1) == 's')
			{
				ans += ft_dprintf_putstr_fd((char *)va_arg(ap, char *), fd);
				fmt+=2;
			}
			if (*(fmt + 1) == 'd')
			{
				ans += ft_dprintf_putnbr_fd((int)va_arg(ap, int), fd);
				fmt+=2;
			}
		}
		else
		{
			write(fd, fmt, 1);
			fmt++;
		}
	}
	va_end(ap);
	return (ans);
}
