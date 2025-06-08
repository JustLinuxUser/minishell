/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signed_int_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/28 14:54:22 by anddokhn          #+#    #+#             */
/*   Updated: 2024/10/14 19:15:14 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdarg.h>
#include "ft_printf.h"
#include "../utils/utils.h"

int	signed_num_digits(ssize_t n)
{
	int		count;

	count = 0;
	while (n != 0)
	{
		count++;
		n /= 10;
	}
	return (count);
}

int	signed_padding_len(ssize_t n, t_fmt_spec spec)
{
	int	digit_count;

	digit_count = signed_num_digits(n);
	if (spec.precision >= digit_count)
	{
		return (spec.precision - digit_count);
	}
	return (0);
}

int	signed_total_len(ssize_t n, t_fmt_spec spec)
{
	int	len;

	len = signed_num_digits(n);
	len += signed_padding_len(n, spec);
	if (n < 0 || (spec.flags & FL_PLUS) || (spec.flags & FL_SPACE))
		len++;
	return (len);
}

t_fmt_spec	normilize_int_spec(t_fmt_spec spec, ssize_t i)
{
	if (spec.precision != -1 || spec.flags & FL_MINUS)
		spec.flags &= ~FL_ZERO;
	if (spec.width != -1 && (spec.flags & FL_ZERO))
	{
		if (i < 0 || (spec.flags & FL_PLUS) || (spec.flags & FL_SPACE))
		{
			spec.precision = spec.width - 1;
			spec.width = -1;
			if (i == 0)
				spec.precision = ft_max(spec.precision, 1);
		}
		else
		{
			spec.precision = spec.width;
			spec.width = -1;
		}
	}
	if (i == 0)
		if (spec.precision == -1)
			spec.precision = 1;
	return (spec);
}
