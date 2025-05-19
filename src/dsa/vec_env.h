/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/19 17:19:03 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_ENV_H
# define VEC_ENV_H
# include <stdbool.h>
# include <stdlib.h>

typedef struct s_env
{
	bool	exported;
	char	*key;
	char	*value;
}	t_env;

typedef struct s_vec_env
{
	size_t	cap;
	size_t	len;
	t_env	*buff;
}	t_vec_env;
void	vec_env_del(t_vec_env *v, size_t i);
int		vec_env_init(t_vec_env *ret);
int		vec_env_double(t_vec_env *v);
int		vec_env_push(t_vec_env *v, t_env el);
int		env_set(t_vec_env *v, t_env el);
t_env	vec_env_pop(t_vec_env *v);
t_env	*vec_env_idx(t_vec_env *v, size_t idx);
void	env_extend(t_vec_env *dest, t_vec_env *src, bool export);
char	*env_get_ifs(t_vec_env *v);
#endif
