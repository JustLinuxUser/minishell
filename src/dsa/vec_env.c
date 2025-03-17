/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 18:37:43 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "vec_env.h"
#include "../libft/utils/utils.h"
#define VEC_ENV_INIT_CAP 10

int	vec_env_init(t_vec_env *ret)
{
	ret->buff = malloc(sizeof(t_env) * VEC_ENV_INIT_CAP);
	if (ret->buff == 0)
		return (1);
	ret->len = 0;
	ret->cap = VEC_ENV_INIT_CAP;
	return (0);
}

int	vec_env_double(t_vec_env *v)
{
	t_env	*temp;
	size_t	i;

	v->cap *= 2;
	temp = malloc(sizeof(t_env) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_env_push(t_vec_env *v, t_env el)
{
	if (v->len == v->cap)
		if (vec_env_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

t_env	vec_env_pop(t_vec_env *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

t_env	*vec_env_idx(t_vec_env *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (&v->buff[idx]);
}
