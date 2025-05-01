/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_simple_command.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:53:24 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/30 22:28:21 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include "../libft/libft.h"

// returns status
int	actually_run(t_state *state, t_vec_str *args)
{
	char	*path_of_exe;
	char	**envp;
	int		status;

	assert(args->len >= 1);
	if (builtin_func(args->buff[0]))
		exit(builtin_func(args->buff[0])(state, *args));
	status = find_cmd_path(state, args->buff[0], &path_of_exe);
	if (status)
		return (status);
	vec_str_push(args, 0);
	envp = get_envp(state, path_of_exe);
	execve(path_of_exe, args->buff, envp);
	err_1_errno(state, args->buff[0]);
	free_all_state(state);
	free_tab(args->buff);
	free_tab(envp);
	free(path_of_exe);
	return (EXE_PERM_DENIED);
}

void	set_up_redirection(t_state *state, t_executable_node *exe)
{
	t_redir	redir;
	size_t	i;

	if (exe->next_infd != -1)
		close(exe->next_infd);
	dup2(exe->outfd, 1);
	dup2(exe->infd, 0);
	i = 0;
	while (i < exe->redirs.len)
	{
		redir = state->redirects.buff[exe->redirs.buff[i++]];
		if (redir.direction_in)
			dup2(redir.fd, 0);
		else
			dup2(redir.fd, 1);
		close(redir.fd);
	}
}

t_exe_res	execute_builtin_cmd_fg(t_state *state, t_executable_cmd *cmd,
	t_executable_node *exe)
{
	int		stdin_bak;
	int		stdout_bak;
	int		status;

	stdin_bak = dup(0);
	stdout_bak = dup(1);
	set_up_redirection(state, exe);
	status = builtin_func(cmd->argv.buff[0])(state, cmd->argv);
	dup2(stdin_bak, 0);
	dup2(stdout_bak, 1);
	close(stdin_bak);
	close(stdout_bak);
	free_executable_cmd(*cmd);
	free_executable_node(exe);
	return (res_status(status));
}

t_exe_res	execute_cmd_bg(t_state *state,
		t_executable_node *exe, t_executable_cmd *cmd)
{
	int	pid;

	pid = fork();
	if (pid == 0)
	{
		default_signal_handlers();
		set_up_redirection(state, exe);
		env_extend(&state->env, &cmd->pre_assigns);
		exit(actually_run(state, &cmd->argv));
	}
	free_executable_cmd(*cmd);
	free_executable_node(exe);
	return (res_pid(pid));
}

t_exe_res	execute_simple_command(t_state *state, t_executable_node *exe)
{
	t_executable_cmd	cmd;

	if (expand_simple_command(state, exe->node, &cmd, &exe->redirs))
	{
		free_executable_cmd(cmd);
		free_executable_node(exe);
		if (g_should_unwind)
			return (res_status(CANCELED));
		return (res_status(AMBIGUOUS_REDIRECT));
	}
	if (cmd.argv.len && builtin_func(cmd.argv.buff[0])
		&& exe->modify_parent_context)
		return (execute_builtin_cmd_fg(state, &cmd, exe));
	else if (cmd.argv.len)
		return (execute_cmd_bg(state, exe, &cmd));
	else
	{
		if (exe->modify_parent_context)
			env_extend(&state->env, &cmd.pre_assigns);
		free_executable_cmd(cmd);
		free_executable_node(exe);
		return (res_status(0));
	}
}
