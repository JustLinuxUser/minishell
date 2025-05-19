/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:11 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/19 17:14:31 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_env(t_state *state, t_vec_str argv)
{
	size_t	i;

	i = 0;
	(void)argv;
	while (i < state->env.len)
	{
		if (state->env.buff[i].exported)
			ft_printf("%s=%s\n", state->env.buff[i].key,
				state->env.buff[i].value);
		++i;
	}
	return (0);
}
