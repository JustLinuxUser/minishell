/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_env2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:16:50 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:16:51 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../minishell.h"
#include "vec_env.h"
#include <stdio.h>

t_env	str_to_env(char *str)
{
	t_env	ret;
	char	*key_pos;

	key_pos = ft_strchr(str, '=') + 1;
	ft_assert(key_pos != 0);
	ret.exported = true;
	ret.key = malloc(key_pos - str);
	ft_strlcpy(ret.key, str, key_pos - str);
	ret.value = ft_strdup(key_pos);
	return (ret);
}

t_vec_env	env_to_vec_env(t_state *state, char **envp)
{
	t_vec_env	ret;

	vec_env_init(&ret);
	while (*envp)
	{
		vec_env_push(&ret, str_to_env(*envp));
		envp++;
	}
	if (state->cwd.len)
		env_set(&ret, (t_env){.key = ft_strdup("PWD"),
			.value = ft_strdup(state->cwd.buff)});
	return (ret);
}

char	**get_envp(t_state*state)
{
	char		**ret;
	size_t		i;
	t_dyn_str	s;

	ret = ft_calloc(state->env.len + 1, sizeof(char *));
	i = 0;
	while (i < state->env.len)
	{
		dyn_str_init(&s);
		dyn_str_pushstr(&s, state->env.buff[i].key);
		dyn_str_push(&s, '=');
		dyn_str_pushstr(&s, state->env.buff[i].value);
		ret[i] = s.buff;
		i++;
	}
	return (ret);
}

t_env	*env_get(t_vec_env *env, char *key)
{
	t_env	*curr;
	size_t	i;

	i = 0;
	while (i < env->len)
	{
		curr = vec_env_idx(env, i);
		if (ft_strcmp(key, curr->key) == 0)
			return (curr);
		i++;
	}
	return (0);
}

char	*env_expand_n(t_state *state, char *key, int len)
{
	t_env	*curr;

	if (ft_strncmp(key, "?", len) == 0 && len == 1)
		return (state->last_cmd_status_s);
	else if (ft_strncmp(key, "$", len) == 0 && state->pid && len == 1)
		return (state->pid);
	curr = env_nget(&state->env, key, len);
	if (curr == 0 || curr->key == 0)
		return (0);
	return (curr->value);
}
