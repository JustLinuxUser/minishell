/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_glob.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/28 18:34:02 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "vec_glob.h"
#include "../libft/utils/utils.h"

int	vec_glob_init(t_vec_glob *ret)
{
	*ret = (t_vec_glob){0};
	return (0);
}

int	vec_glob_double(t_vec_glob *v)
{
	t_glob	*temp;
	size_t	i;

	v->cap = v->cap * 2 + 1;
	temp = malloc(sizeof(t_glob) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_glob_push(t_vec_glob *v, t_glob el)
{
	if (v->len == v->cap)
		if (vec_glob_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

t_glob	vec_glob_pop(t_vec_glob *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

t_glob	vec_glob_idx(t_vec_glob *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (v->buff[idx]);
}
