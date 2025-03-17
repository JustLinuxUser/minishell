/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_str.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 18:38:38 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "vec_str.h"
#include "../libft/utils/utils.h"
#define VEC_STR_INIT_CAP 10

int	vec_str_init(t_vec_str *ret)
{
	ret->buff = malloc(sizeof(char *) * VEC_STR_INIT_CAP);
	if (ret->buff == 0)
		return (1);
	ret->len = 0;
	ret->cap = VEC_STR_INIT_CAP;
	return (0);
}

int	vec_str_double(t_vec_str *v)
{
	char *	*temp;
	size_t	i;

	v->cap *= 2;
	temp = malloc(sizeof(char *) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_str_push(t_vec_str *v, char * el)
{
	if (v->len == v->cap)
		if (vec_str_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

char *	vec_str_pop(t_vec_str *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

char *	vec_str_idx(t_vec_str *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (v->buff[idx]);
}
