/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:39:33 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/24 23:28:03 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

void	init_arg(t_state *state, char **argv)
{
	if (!argv[2])
	{
		ft_eprintf("%s: -c: option requires an argument\n",
			state->base_context);
		free_all_state(state);
		exit(SYNTAX_ERR);
	}
	dyn_str_pushstr(&state->readline_buff.buff, argv[2]);
	dyn_str_push(&state->readline_buff.buff, '\n');
	buff_readline_update(&state->readline_buff);
	state->readline_buff.should_update_context = true;
	state->input_method = INP_ARG;
}

void	init_file(t_state *state, char **argv)
{
	int	fd;

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
	{
		ft_eprintf("%s: %s: %s\n", state->base_context,
			argv[1], strerror(errno));
		free_all_state(state);
		if (errno == EISDIR)
			exit(127);
		exit(EXE_BASE_ERROR + errno);
	}
	dyn_str_append_fd(fd, &state->readline_buff.buff);
	close(fd);
	dyn_str_push(&state->readline_buff.buff, '\n');
	buff_readline_update(&state->readline_buff);
	state->readline_buff.should_update_context = true;
	free(state->base_context);
	free(state->context);
	state->base_context = ft_strdup(argv[1]);
	state->context = ft_strdup(argv[1]);
	state->input_method = INP_FILE;
}

void	init_stdin_notty(t_state *state)
{
	state->input_method = INP_STDIN_NOTTY;
	state->readline_buff.should_update_context = true;
}

void	init_cwd(t_state *state)
{
	char	*cwd;

	dyn_str_init(&state->cwd);
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		dyn_str_pushstr(&state->cwd, cwd);
	}
	else 
	{
	ft_eprintf("shell-init: error retrieving current directory:"
		" getcwd: cannot access parent directories:"
		" No such file or directory\nsh: 0: "
		"getcwd() failed: No such file or directory\n");
	}
	free(cwd);
}

void	init_setup(t_state *state, char **argv, char **envp)
{
	set_unwind_sig();
	*state = (t_state){0};
	state->pid = getpid_hack();
	state->context = ft_strdup(argv[0]);
	state->base_context = ft_strdup(argv[0]);
	set_cmd_status(state, res_status(0));
	state->last_cmd_status_res = res_status(0);
	init_cwd(state);
	state->env = env_to_vec_env(state, envp);
	if (argv[1] && ft_strcmp(argv[1], "-c") == 0)
		init_arg(state, argv);
	else if (argv[1])
		init_file(state, argv);
	else if (!isatty(0) || !isatty(1))
		init_stdin_notty(state);
	else
		init_history(state);
}
