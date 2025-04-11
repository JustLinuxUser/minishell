/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/11 23:31:59 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "vec_env.h"
#include "../libft/utils/utils.h"
#include "../libft/libft.h"

int	vec_env_init(t_vec_env *ret)
{
	*ret = (t_vec_env){0};
	return (0);
}

int	vec_env_double(t_vec_env *v)
{
	t_env	*temp;
	size_t	i;

	v->cap = v->cap * 2 + 1;
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

void vec_env_del(t_vec_env *v, size_t i)
{
	ft_assert( i < v->len);
	free(v->buff[i].key);
	free(v->buff[i].value); 
	ft_memmove(&v->buff[i], &v->buff[i+1], (sizeof(t_env)*(v->len-i-1 )));
	v->len = (v->len - 1);
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
