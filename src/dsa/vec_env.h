/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2025/02/22 00:06:11 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_ENV_H
# define VEC_ENV_H
#include <stdbool.h>
# include <stdlib.h>

typedef struct s_env {
	bool exported;
	char *key;
	char *value;
} t_env;

typedef struct s_vec_env
{
	size_t	cap;
	size_t	len;
	t_env		*buff;
}	t_vec_env;
int	vec_env_init(t_vec_env *ret);
int	vec_env_double(t_vec_env *v);
int	vec_env_push(t_vec_env *v, t_env el);
t_env	vec_env_pop(t_vec_env *v);
t_env	*vec_env_idx(t_vec_env *v, size_t idx);
#endif
