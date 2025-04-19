/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env4.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:17:43 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:17:57 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec_env.h"
#include "../libft/utils/utils.h"

t_env	*vec_env_idx(t_vec_env *v, size_t idx)
{
	ft_assert(idx < v->len);
	return (&v->buff[idx]);
}
