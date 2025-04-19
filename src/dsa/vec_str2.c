/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_str2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 22:35:07 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 22:35:11 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vec_str.h"
#include <stddef.h>

void	free_vec_str(t_vec_str *args)
{
	size_t	i;

	i = 0;
	while (i < args->len)
	{
		free(args->buff[i]);
		i++;
	}
	free(args->buff);
	vec_str_init(args);
}
