/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_nd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/02/09 05:53:19 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "minishell.h"
#include "libft/utils/utils.h"

int	vec_nd_init(t_vec_nd *ret)
{
	*ret = (t_vec_nd){0};
	return (0);
}

int	vec_nd_double(t_vec_nd *v)
{
	t_ast_node	*temp;
	int	i;

	v->cap += 1;
	v->cap *= 2;
	temp = malloc(sizeof(t_ast_node) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < (int)v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_nd_push(t_vec_nd *v, t_ast_node el)
{
	if (v->len == v->cap)
		if (vec_nd_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

t_ast_node	vec_nd_pop(t_vec_nd *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

t_ast_node	*vec_nd_idx(t_vec_nd *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (&v->buff[idx]);
}
