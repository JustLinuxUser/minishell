/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:15 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/25 17:59:46 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

void	exit_clean(t_state *state, int code)
{
	char	*pid_s;

	pid_s = getpid_hack();
	if (pid_s && state->pid && ft_strcmp(state->pid, pid_s) == 0)
	{
		manage_history(state);
		free_all_state(state);
	}
	free(pid_s);
	exit(code);
}

int	builtin_exit(t_state *state, t_vec_str argv)
{
	int	ret;

	if (state->input_method == INP_READLINE)
		ft_eprintf("exit\n");
	if (argv.len == 1)
		exit_clean(state, 0);
	if (ft_checked_atoi(argv.buff[1], &ret, 42))
	{
		ft_eprintf("%s: %s: %s: numeric argument required\n", state->context,
			argv.buff[0], argv.buff[1]);
		exit_clean(state, 2);
	}
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->context,
			argv.buff[0]);
		return (1);
	}
	exit_clean(state, ret);
	return (0);
}
