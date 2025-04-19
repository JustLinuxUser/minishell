/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:38:06 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <assert.h>
#include "dsa/vec_exe_res.h"
#include "minishell.h"
#include <stdbool.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <stdlib.h>
#include "libft/libft.h"

#include "libft/dsa/dyn_str.h"

int	main(int argc, char **argv, char **envp)
{
	t_state	state;

	(void)argc;
	init_setup(&state, argv, envp);
	while (!state.should_exit)
	{
		dyn_str_init(&state.input);
		g_should_unwind = 0;
		parse_and_execute_input(&state);
		free_redirects(&state.redirects);
		free_ast(&state.tree);
		free(state.input.buff);
		state.input = (t_dyn_str){};
	}
	free_env(&state.env);
	free_all_state(&state);
	forward_exit_status(state.last_cmd_status_res);
}
