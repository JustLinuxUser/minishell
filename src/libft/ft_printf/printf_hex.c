/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printf_hex.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 15:08:25 by anddokhn          #+#    #+#             */
/*   Updated: 2024/09/28 21:16:17 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdarg.h>
#include "ft_printf.h"

void	printf_hex_lowcase(va_list args, t_fmt_spec spec, t_buffer *buff)
{
	unsigned int	i;
	int				pad_len;
	int				total_len;
	int				num_len;

	if (spec.t != FMT_HEX_LOWER)
		return ;
	i = va_arg(args, unsigned int);
	spec = normalize_hex_spec(spec, i);
	total_len = hex_total_len(i, spec);
	pad_len = hex_padding_len(i, spec);
	num_len = hex_digits(i);
	if (!(spec.flags & FL_MINUS) && spec.width != -1)
		writer_padn(buff, ' ', spec.width - total_len);
	writer_hex_lowcase(buff, i, spec.flags & FL_POUND, pad_len + num_len);
	if (spec.flags & FL_MINUS && spec.width != -1)
		writer_padn(buff, ' ', spec.width - total_len);
}

void	printf_hex_upcase(va_list args, t_fmt_spec spec, t_buffer *buff)
{
	unsigned int	i;
	int				pad_len;
	int				total_len;
	int				num_len;

	if (spec.t != FMT_HEX_UPPER)
		return ;
	i = va_arg(args, unsigned int);
	spec = normalize_hex_spec(spec, i);
	total_len = hex_total_len(i, spec);
	pad_len = hex_padding_len(i, spec);
	num_len = hex_digits(i);
	if (!(spec.flags & FL_MINUS) && spec.width != -1)
		writer_padn(buff, ' ', spec.width - total_len);
	writer_hex_upcase(buff, i, spec.flags & FL_POUND, pad_len + num_len);
	if (spec.flags & FL_MINUS && spec.width != -1)
		writer_padn(buff, ' ', spec.width - total_len);
}
