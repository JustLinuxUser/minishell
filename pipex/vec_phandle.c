/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_phandle.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 03:59:51 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "vec_phandle.h"
#include "../libft/utils/utils.h"
#define VEC_PHANDLE_INIT_CAP 10

int	vec_phandle_init(t_vec_phandle *ret)
{
	ret->buff = malloc(sizeof(t_phandle) * VEC_PHANDLE_INIT_CAP);
	if (ret->buff == 0)
		return (1);
	ret->len = 0;
	ret->cap = VEC_PHANDLE_INIT_CAP;
	return (0);
}

int	vec_phandle_double(t_vec_phandle *v)
{
	t_phandle	*temp;
	size_t		i;

	v->cap *= 2;
	temp = malloc(sizeof(t_phandle) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_phandle_push(t_vec_phandle *v, t_phandle el)
{
	if (v->len == v->cap)
		if (vec_phandle_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

t_phandle	vec_phandle_pop(t_vec_phandle *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

t_phandle	vec_phandle_idx(t_vec_phandle *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (v->buff[idx]);
}
