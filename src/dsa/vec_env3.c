/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:16:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:17:03 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../minishell.h"
#include "vec_env.h"
#include <stdio.h>

char	*env_expand(t_state *state, char *key)
{
	return (env_expand_n(state, key, ft_strlen(key)));
}

int	env_set(t_vec_env *env, t_env new)
{
	t_env	*old;

	ft_assert(new.key != 0);
	old = env_get(env, new.key);
	if (old)
	{
		free(old->value);
		free(new.key);
		old->value = new.value;
		old->exported = new.exported;
	}
	else
		return (vec_env_push(env, new));
	return (0);
}

void	env_extend(t_vec_env *dest, t_vec_env *src)
{
	while (src->len)
	{
		env_set(dest, vec_env_pop(src));
	}
	free(src->buff);
	vec_env_init(src);
}

t_env	*env_nget(t_vec_env *env, char *key, int len)
{
	t_env	*curr;
	int		i;

	i = env->len - 1;
	while (i >= 0)
	{
		curr = vec_env_idx(env, i);
		if (ft_strncmp(key, curr->key, len) == 0 && curr->key[len] == 0)
			return (curr);
		i--;
	}
	return (0);
}

void	free_env(t_vec_env *env)
{
	size_t	i;
	t_env	curr;

	i = 0;
	while (i < env->len)
	{
		curr = env->buff[i];
		free(curr.key);
		free(curr.value);
		i++;
	}
	free(env->buff);
	env->buff = 0;
}
