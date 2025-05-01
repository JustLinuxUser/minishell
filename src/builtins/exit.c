/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:15 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 22:17:22 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_exit(t_state *state, t_vec_str argv)
{
	int	ret;

	if (state->input_method == INP_READLINE)
		ft_eprintf("exit\n");
	if (argv.len == 1)
	{
		free_all_state(state);
		exit(0);
	}
	if (ft_checked_atoi(argv.buff[1], &ret, 35))
	{
		ft_eprintf("%s: %s: %s: numeric argument required\n", state->context,
			argv.buff[0], argv.buff[1]);
		free_all_state(state);
		exit(2);
	}
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->context,
			argv.buff[0]);
		free_all_state(state);
		return (1);
	}
	exit(ret);
}

// exit no imprime ext si no estoy en readline