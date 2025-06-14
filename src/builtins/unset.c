/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   unset.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:23 by armgonza          #+#    #+#             */
/*   Updated: 2025/04/29 23:25:24 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

void	try_unset(t_state *state, char *key)
{
	t_env	*e;

	e = env_get(&state->env, key);
	if (e)
		vec_env_del(&state->env, e - state->env.buff);
}

int	builtin_unset(t_state *state, t_vec_str argv)
{
	size_t	i;

	i = 1;
	while (i < argv.len)
	{
		try_unset(state, argv.buff[i]);
		i++;
	}
	return (0);
}
