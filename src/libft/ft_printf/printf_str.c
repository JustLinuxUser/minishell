/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_str.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 15:12:35 by anddokhn          #+#    #+#             */
/*   Updated: 2024/10/14 18:34:20 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"
#include "../libft.h"
#include <stdarg.h>

void	printf_string(va_list args, t_fmt_spec spec, t_buffer *buff)
{
	char	*s;
	int		len;

	if (spec.t != FMT_STRING)
		return ;
	s = va_arg(args, char *);
	if (s == 0)
	{
		s = "(null)";
		if (spec.precision < 6 && spec.precision != -1)
			spec.precision = 0;
		else if (spec.precision != -1)
			spec.precision = 6;
	}
	if (spec.precision != -1)
		len = ft_strnlen(s, spec.precision);
	else
		len = ft_strlen(s);
	if (spec.flags & FL_MINUS)
		writer_strn(buff, s, len);
	if (spec.width)
		writer_padn(buff, ' ', spec.width - len);
	if (!(spec.flags & FL_MINUS))
		writer_strn(buff, s, len);
}

void	printf_char(va_list args, t_fmt_spec spec, t_buffer *buff)
{
	char	c;

	if (spec.t != FMT_CHAR)
		return ;
	c = va_arg(args, int);
	if (spec.flags & FL_MINUS)
		writer_char(buff, c);
	if (spec.width != -1)
		writer_padn(buff, ' ', spec.width - 1);
	if (!(spec.flags & FL_MINUS))
		writer_char(buff, c);
}
