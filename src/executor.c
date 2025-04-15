/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/15 18:33:05 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_str.h"
#include "minishell.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>

#include <readline/readline.h>
#include <string.h>
#include <unistd.h>

#include "dsa/vec_env.h"
#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "dsa/vec_exe_res.h"

char	*exe_path(char **path_dirs, char *exe_name)
{
	t_dyn_str	temp;
	int			i;

	if (ft_strchr(exe_name, '/') || ft_strlen(exe_name) == 0)
		return (ft_strdup(exe_name));
	dyn_str_init(&temp);
	i = -1;
	while (path_dirs[++i])
	{
		dyn_str_clear(&temp);
		dyn_str_pushstr(&temp, path_dirs[i]);
		dyn_str_push(&temp, '/');
		dyn_str_pushstr(&temp, exe_name);
		if (access(temp.buff, F_OK) == 0)
			break ;
		else if (path_dirs[i + 1] == 0)
			return (free(temp.buff), NULL);
	}
	if (temp.len > 0)
		return (temp.buff);
	return (free(temp.buff), NULL);
}

void	err_cmd_not_found(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: command not found\n", state->context, cmd);
}

void	err_no_path_var(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: No such file or directory\n", state->context, cmd);
}

void	err_cmd_other(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: %s\n", state->context, cmd, strerror(errno));
}

char	*find_cmd_path(t_state *state, t_vec_str *args)
{
	char	*path;
	char	**path_dirs;
	char	*path_of_exe;

	path = env_expand(state, "PATH");
	if (!path)
	{
		err_no_path_var(state, args->buff[0]);
		free_all_state(state);
		return (0);
	}
	path_dirs = ft_split(path, ':');
	path_of_exe = exe_path(path_dirs, args->buff[0]);
	free_tab(path_dirs);
	if (!path_of_exe)
	{
		err_cmd_not_found(state, args->buff[0]);
		free_all_state(state);
		return (0);
	}
	return (path_of_exe);
}

// returns status
int	actually_run(t_state *state, t_vec_str *args)
{
	char	*path_of_exe;
	char	**envp;

	assert(args->len >= 1);
	if (builtin_func(args->buff[0]))
		exit(builtin_func(args->buff[0])(state, *args));
	path_of_exe = find_cmd_path(state, args);
	if (!path_of_exe)
		return (COMMAND_NOT_FOUND);
	vec_str_push(args, 0);
	envp = get_envp(state);
	execve(path_of_exe, args->buff, envp);
	err_cmd_other(state, args->buff[0]);
	free_all_state(state);
	free_tab(args->buff);
	free_tab(envp);
	free(path_of_exe);
	return (EXE_PERM_DENIED);
}

void	free_executable_cmd(executable_cmd_t cmd)
{
	size_t	i;

	i = 0;
	while (i < cmd.pre_assigns.len)
	{
		free(cmd.pre_assigns.buff[i].value);
		free(cmd.pre_assigns.buff[i].key);
		i++;
	}
	i = 0;
	while (i < cmd.argv.len)
	{
		free(cmd.argv.buff[i]);
		i++;
	}
	free(cmd.pre_assigns.buff);
	free(cmd.argv.buff);
}

void	free_executable_node(t_state *state, t_executable_node *node)
{
	t_redir	*curr;
	size_t	i;

	i = 0;
	while (i < node->redirs.len)
	{
		curr = &state->redirects.buff[node->redirs.buff[i]];
		if (curr->should_delete)
			unlink(curr->fname);
		free(curr->fname);
		ft_memset(curr, 0, sizeof(*curr));
		i++;
	}
	free(node->redirs.buff);
}

void	set_up_redirection(t_state *state, t_executable_node *exe)
{
	t_redir	redir;
	size_t	i;

	if (exe->next_infd)
		close(exe->next_infd);
	if (exe->outfd != 1)
	{
		dup2(exe->outfd, 1);
		close(exe->outfd);
	}
	if (exe->infd != 0)
	{
		dup2(exe->infd, 0);
		close(exe->infd);
	}
	i = 0;
	while (i < exe->redirs.len)
	{
		redir = state->redirects.buff[exe->redirs.buff[i]];
		if (redir.direction_in)
			dup2(redir.fd, 0);
		else
			dup2(redir.fd, 1);
		close(redir.fd);
		i++;
	}
}

t_exe_res	execute_builtin_cmd_fg(t_state *state, executable_cmd_t *cmd, t_executable_node *exe)
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
	free_executable_node(state, exe);
	return (res_status(status));
}

t_exe_res	execute_cmd_bg(t_state *state, t_executable_node *exe, executable_cmd_t *cmd)
{
	int	pid;

	pid = fork();
	if (pid == 0)
	{
		die_on_sig();
		set_up_redirection(state, exe);
		env_extend(&state->env, &cmd->pre_assigns);
		exit(actually_run(state, &cmd->argv));
	}
	free_executable_cmd(*cmd);
	free_executable_node(state, exe);
	return (res_pid(pid));
}

t_exe_res	execute_simple_command(t_state* state, t_executable_node *exe)
{
	executable_cmd_t	cmd;

	if (expand_simple_command(state, exe->node, &cmd, &exe->redirs))
	{
		free_executable_cmd(cmd);
		free_executable_node(state, exe);
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
		free_executable_node(state, exe);
		return (res_status(0));
	}
}

void	set_up_redir_pipeline_child(bool is_last, t_executable_node *exe,
	t_executable_node *curr_exe, int (*pp)[2])
{
	if (!is_last)
	{
		if (pipe(*pp))
			critical_error_errno();
		curr_exe->outfd = (*pp)[1];
		curr_exe->next_infd = (*pp)[0];
	}
	else
	{
		curr_exe->next_infd = 0;
		curr_exe->outfd = dup(exe->outfd);
	}
}

void	execute_pipeline_children(t_state *state, t_executable_node *exe, t_vec_exe_res *results)
{
	size_t				i;
	t_executable_node	curr_exe;
	int					pp[2];

	i = 0;
	curr_exe = (t_executable_node){};
	curr_exe.infd = dup(exe->infd);
	curr_exe.modify_parent_context = exe->node->children.len == 1;
	while (i < exe->node->children.len)
	{
		curr_exe.node = vec_nd_idx(&exe->node->children, i);
		assert(curr_exe.node->node_type == AST_COMMAND);
		set_up_redir_pipeline_child(i == exe->node->children.len - 1,
			exe, &curr_exe, &pp);
		vec_exe_res_push(results, execute_command(state, &curr_exe));
		close(curr_exe.outfd);
		close(curr_exe.infd);
		curr_exe.infd = pp[0];
		i++;
	}
}

// Always returns status
t_exe_res	execute_pipeline(t_state *state, t_executable_node *exe)
{
	size_t				i;
	t_vec_exe_res		results;
	t_exe_res			res;

	results = (t_vec_exe_res){};
	execute_pipeline_children(state, exe, &results);
	res = res_status(0);
	i = 0;
	while (i < results.len)
	{
		res = vec_exe_res_idx(&results, i);
		exe_res_set_status(&res);
		i++;
	}
	free(results.buff);
	return (res);
}

bool	should_execute(t_exe_res prev_status, t_tt prev_op)
{
	if (prev_status.c_c)
		return (false);
	ft_assert(prev_status.status != -1);
	assert(prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE
		|| prev_op == TT_AND || prev_op == TT_OR);
	if (prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE)
		return (true);
	if (prev_op == TT_AND && prev_status.status == 0)
		return (true);
	if (prev_op == TT_OR && prev_status.status != 0)
		return (true);
	return (false);
}

t_exe_res	execute_simple_list(t_state *state, t_executable_node *exe)
{
	t_exe_res			status;
	t_tt				op;
	t_executable_node	exe_curr;
	size_t				i;

	status = res_status(0);
	op = TT_SEMICOLON;
	i = 0;
	while (i < exe->node->children.len)
	{
		exe_curr = *exe;
		exe_curr.node = &exe->node->children.buff[i];
		if (should_execute(status, op))
			status = execute_tree_node(state, &exe_curr);
		i++;
		if (i < exe->node->children.len)
			op = exe->node->children.buff[i].token.tt;
		i++;
	}
	return (status);
}

void	forward_exit_status(t_exe_res res)
{
	if (res.c_c)
	{
		default_signal_handlers();
		kill(0, SIGINT);
	}
	exit(res.status);
}

// returns pid
t_exe_res	execute_subshell(t_state *state, t_executable_node *exe)
{
	int	pid;

	pid = fork();
	if (pid == 0)
	{
		die_on_sig();
		set_up_redirection(state, exe);
		exe->node = &exe->node->children.buff[0];
		forward_exit_status(execute_tree_node(state, exe));
	}
	if (pid < 0)
		critical_error_errno();
	return (res_pid(pid));
}

// gives back pid;
t_exe_res	execute_command(t_state* state, t_executable_node *exe)
{
	t_ast_node	*curr;
	size_t		i;
	int			redir_idx;

	assert(exe->node->children.len >= 1);
	if (exe->node->children.buff[0].node_type == AST_SIMPLE_COMMAND)
	{
		exe->node = &exe->node->children.buff[0];
		return (execute_simple_command(state, exe));
	}
	assert(exe->node->children.buff[0].node_type == AST_SUBSHELL);
	i = 0;
	while (++i < exe->node->children.len)
	{
		curr = vec_nd_idx(&exe->node->children, i);
		assert(curr->node_type == AST_REDIRECT);
		if (redirect_from_ast_redir(state, curr, &redir_idx))
			return (res_status(AMBIGUOUS_REDIRECT));
		vec_int_push(&exe->redirs, redir_idx);
	}
	exe->node = vec_nd_idx(&exe->node->children, 0);
	exe->modify_parent_context = true;
	return (execute_subshell(state, exe));
}

// always returns status
t_exe_res	execute_tree_node(t_state *state, t_executable_node *exe)
{
	t_exe_res		status;
	t_ast_t	t;

	t = exe->node->node_type;
	status = res_status(0);
	if (t == AST_COMMAND_PIPELINE)
		status = execute_pipeline(state, exe);
	else if (t == AST_SIMPLE_LIST || t == AST_COMPOUND_LIST)
		status = execute_simple_list(state, exe);
	else
		assert("Unreachable" == 0);
	free(state->last_cmd_status);
	state->last_cmd_status = ft_itoa(status.status);
	return (status);
}

void	execute_top_level(t_state *state)
{
	t_executable_node	exe;
	t_exe_res			res;

	exe = (t_executable_node){.infd = 0, .outfd = 1, .node = &state->tree,
		.modify_parent_context = true};
	state->heredoc_idx = 0;
	if (!g_should_unwind)
		gather_heredocs(state, &state->tree);
	if (!g_should_unwind)
	{
		res = execute_tree_node(state, &exe);
		if (res.c_c)
			ft_printf("\n");
	}
}
