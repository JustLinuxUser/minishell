/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_nd2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:28:33 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:28:34 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	vec_nd_push_vec(t_vec_nd *ret, t_vec_nd *second)
{
	int	i;

	i = -1;
	while (++i < (int)second->len)
	{
		vec_nd_push(ret, *vec_nd_idx(second, i));
	}
}

void	vec_nd_free(t_vec_nd *ret)
{
	free(ret->buff);
	*ret = (t_vec_nd){0};
}
