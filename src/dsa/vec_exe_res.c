/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_exe_res.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:51:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/19 04:51:40 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <sys/wait.h>
#include "vec_exe_res.h"
#include "../libft/utils/utils.h"

int	vec_exe_res_init(t_vec_exe_res *ret)
{
	*ret = (t_vec_exe_res){0};
	return (0);
}

int	vec_exe_res_double(t_vec_exe_res *v)
{
	t_exe_res	*temp;
	size_t	i;

	v->cap = v->cap * 2 + 1;
	temp = malloc(sizeof(t_exe_res) * v->cap);
	if (temp == 0)
		return (1);
	i = -1;
	while (++i < v->len)
		temp[i] = v->buff[i];
	free(v->buff);
	v->buff = temp;
	return (0);
}

int	vec_exe_res_push(t_vec_exe_res *v, t_exe_res el)
{
	if (v->len == v->cap)
		if (vec_exe_res_double(v))
			return (1);
	v->buff[v->len++] = el;
	return (0);
}

t_exe_res	vec_exe_res_pop(t_vec_exe_res *v)
{
	ft_assert(v->len > 0);
	return (v->buff[--v->len]);
}

t_exe_res	vec_exe_res_idx(t_vec_exe_res *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (v->buff[idx]);
}

t_exe_res res_status(int status)
{
	return (t_exe_res){.status = status, .pid = -1};
}

t_exe_res res_pid(int pid)
{
	return (t_exe_res){.status = -1, .pid = pid};
}

int exe_res_to_status(t_exe_res res)
{
	if (res.status != -1)
		return (res.status);
	waitpid(res.pid, &res.status, 0);
	return (res.status);
}
