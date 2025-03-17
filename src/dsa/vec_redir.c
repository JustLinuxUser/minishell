/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_redir.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 18:38:29 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "../minishell.h"
#include "../libft/utils/utils.h"
#define VEC_REDIR_INIT_CAP 10

int	vec_redir_init(t_vec_redir *ret)
{
	ret->buff = malloc(sizeof(redir_t) * VEC_REDIR_INIT_CAP);
	if (ret->buff == 0)
		return (1);
	ret->len = 0;
	ret->cap = VEC_REDIR_INIT_CAP;
	return (0);
}

int	vec_redir_double(t_vec_redir *v)
{
	redir_t	*temp;
	size_t	i;

	v->cap *= 2;
	temp = malloc(sizeof(redir_t) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_redir_push(t_vec_redir *v, redir_t el)
{
	if (v->len == v->cap)
		if (vec_redir_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

redir_t	vec_redir_pop(t_vec_redir *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

redir_t	vec_redir_idx(t_vec_redir *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (v->buff[idx]);
}
