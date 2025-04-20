/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env4.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:17:43 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 20:23:21 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec_env.h"
#include "../minishell.h"
#include "../libft/utils/utils.h"

t_env	*vec_env_idx(t_vec_env *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (&v->buff[idx]);
}

char	*env_get_ifs(t_vec_env *v)
{
	t_env	*e;

	e = env_get(v, "IFS");
	if (!e)
		return (" \t\n");
	return (e->value);
}
