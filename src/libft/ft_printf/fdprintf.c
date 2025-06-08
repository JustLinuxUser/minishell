/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fdprintf.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/05 13:53:55 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/05 14:15:03 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include "../utils/utils.h"

int	ft_vfdprintf(int fd, const char *str, va_list args)
{
	int				i;
	t_fmt_spec		spec;
	t_buffer		buff;

	i = 0;
	writer_init(&buff);
	buff.out_fd = fd;
	while (str[i])
	{
		spec = parse(str, &i, args);
		print_spec(args, spec, &buff);
	}
	if (buff.buff)
	{
		ft_fdputmem(fd, buff.buff, buff.len);
		free(buff.buff);
	}
	return (buff.total_put_in);
}

int	ft_fdprintf(int fd, const char *str, ...)
{
	va_list	args;
	int		ret;

	va_start(args, str);
	ret = ft_vfdprintf(fd, str, args);
	va_end(args);
	return (ret);
}
