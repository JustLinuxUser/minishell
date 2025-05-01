/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pwd.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:20 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 23:08:48 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_pwd(t_state *state, t_vec_str argv)
{
	(void)argv;
	if (state->cwd.buff == NULL)
		ft_eprintf("pwd: error retrieving current directory:"
			" getcwd: cannot access parent directories:"
			" No such file or directory\n");
	else
		ft_printf("%s\n", state->cwd.buff);
	return (0);
}
