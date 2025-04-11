/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/11 22:17:17 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_str.h"
#include "minishell.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include <readline/readline.h>
#include <string.h>
#include <sys/wait.h>
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

void	err_cmd_not_found(t_state* state, char* cmd)
{
    ft_eprintf("%s: %s: command not found\n", state->argv[0], cmd);
}

void err_no_path_var(t_state* state, char* cmd) {
    ft_eprintf("%s: %s: No such file or directory\n", state->argv[0], cmd);
}

void err_cmd_other(t_state* state, char* cmd) {
    ft_eprintf("%s: %s: %s\n", state->argv[0], cmd, strerror(errno));
}

// returns status
int actually_run(t_state* state, t_vec_str* args) {
    assert(args->len >= 1);
    ft_eprintf("executing cmd: %s\n", args->buff[0]);
	if (builtin_func(args->buff[0])) {
		exit(builtin_func(args->buff[0])(state, *args));
	}
    char* path = env_expand(state, "PATH");
    if (!path) {
        err_no_path_var(state, args->buff[0]);
		free_all_state(state);
        return (COMMAND_NOT_FOUND);
    }
    char** path_dirs = ft_split(path, ':');
    char* path_of_exe = exe_path(path_dirs, args->buff[0]);

    free_tab(path_dirs);
    if (!path_of_exe) {
        err_cmd_not_found(state, args->buff[0]);
		free_all_state(state);
        return (COMMAND_NOT_FOUND);
    }
    vec_str_push(args, 0);  // nullterm
    char** envp = get_envp(state);
    execve(path_of_exe, args->buff, envp);
	free_all_state(state);
	// err_cmd_other(state, args->buff[0]);
	// free_tab(args->buff);
	// free_tab(envp);
	// free_ast(state->tree);
	free(path_of_exe);
    return (EXE_PERM_DENIED);
}

void free_executable_cmd(executable_cmd_t cmd)
{
    for (size_t i = 0; i < cmd.pre_assigns.len; i++) {
        free(cmd.pre_assigns.buff[i].value);
        free(cmd.pre_assigns.buff[i].key);
    }

    for (size_t i = 0; i < cmd.argv.len; i++) {
        free(cmd.argv.buff[i]);
    }
    free(cmd.pre_assigns.buff);
    free(cmd.argv.buff);
}

void free_executable_node(t_state *state, t_executable_node *node)
{
	t_redir	*curr;

    for (size_t i = 0; i < node->redirs.len; i++) {
		printf("i: %i\n", node->redirs.buff[i]);
		printf("len: %zu\n", state->redirects.len);
		curr = &state->redirects.buff[node->redirs.buff[i]];
		if (curr->should_delete)
			unlink(curr->fname);
        free(curr->fname);
		ft_memset(curr, 0, sizeof(*curr));
    }
    free(node->redirs.buff);
}

void set_up_redirection(t_state *state, t_executable_node *exe)
{
	t_redir redir;

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
	for (size_t i = 0; i < exe->redirs.len; i++) {
		redir = state->redirects.buff[exe->redirs.buff[i]];
		if (redir.direction_in) {
			dup2(redir.fd, 0);
		} else {
			dup2(redir.fd, 1);
		}
		close(redir.fd);
	}
}

t_exe_res execute_simple_command(t_state* state, t_executable_node *exe) {
	executable_cmd_t	cmd;
	if (expand_simple_command(state, exe->node, &cmd, &exe->redirs))
	{
		free_executable_cmd(cmd);
		free_executable_node(state, exe);
		if (g_should_unwind)
			return res_status(CANCELED);
		return (res_status(AMBIGUOUS_REDIRECT));
	}
    for (size_t i = 0; i < cmd.pre_assigns.len; i++) {
        printf("env: >%s=%s<\n", cmd.pre_assigns.buff[i].key,
               cmd.pre_assigns.buff[i].value);
    }

    for (size_t i = 0; i < cmd.argv.len; i++) {
        printf("arg: >%s<\n", cmd.argv.buff[i]);
    }

	if (cmd.argv.len && builtin_func(cmd.argv.buff[0]) && exe->modify_parent_context) {
		int stdin_bak = dup(0);
		int stdout_bak = dup(1);
		set_up_redirection(state, exe);

		int status = builtin_func(cmd.argv.buff[0])(state, cmd.argv);

		dup2(stdin_bak, 0);
		dup2(stdout_bak, 1);
		close(stdin_bak);
		close(stdout_bak);
		free_executable_cmd(cmd);
		free_executable_node(state, exe);
		return (res_status(status));
	}
	else if (cmd.argv.len) {
		int pid = fork();
		if (pid == 0) {
			die_on_sig();
			set_up_redirection(state, exe);
			env_extend(&state->env, &cmd.pre_assigns);
			exit(actually_run(state, &cmd.argv));
		}
		free_executable_cmd(cmd);
		free_executable_node(state, exe);
		return (res_pid(pid));
	} else {
		if (exe->modify_parent_context)
			env_extend(&state->env, &cmd.pre_assigns);
		free_executable_cmd(cmd);
		free_executable_node(state, exe);
		return (res_status(0));
	}
}

// Always returns status
int execute_pipeline(t_state* state, t_executable_node *exe) {
	t_executable_node curr_exe = {};

    curr_exe.infd = dup(exe->infd);
    // curr_exe.outfd = dup(exe.outfd);
	curr_exe.modify_parent_context = exe->node->children.len == 1; // if its just one, it's ok

    int pp[2];
    size_t i = 0;
    t_vec_exe_res results = {};
    t_exe_res res;

	for (; i < exe->node->children.len - 1; i++) {
		curr_exe.node = vec_nd_idx(&exe->node->children, i);
		if (curr_exe.node->node_type == AST_COMMAND) {
			pipe(pp);
			curr_exe.outfd = pp[1];
			curr_exe.next_infd = pp[0];
			res = execute_command(state, &curr_exe);
			close(curr_exe.outfd);
			vec_exe_res_push(&results, res);
			close(curr_exe.outfd);
			close(curr_exe.infd);
			curr_exe.infd = pp[0];
		} else {
			ft_eprintf("Got unexpected: \n");
			print_node(*curr_exe.node);
			assert("Unimplemented" == 0);
		}
	}
	if (exe->node->children.buff[i].node_type == AST_COMMAND) {
		curr_exe.node = vec_nd_idx(&exe->node->children, i);
		curr_exe.next_infd = 0;
		curr_exe.outfd = dup(exe->outfd);
		res = execute_command(state, &curr_exe);
		close(curr_exe.outfd);
		close(curr_exe.infd);
		vec_exe_res_push(&results, res);
	} else {
		assert("Unimplemented" == 0);
	}
	int status;
	i = 0;
	while (i < results.len) {
		res = vec_exe_res_idx(&results, i);
		status = exe_res_to_status(res);
		i++;
	}
	free(results.buff);
	return (status);
	return (0);
}

int execute_tree_node(t_state* state, t_executable_node *exe);

bool should_execute(int prev_status, t_tt prev_op) {
    assert(prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE ||
           prev_op == TT_AND || prev_op == TT_OR);
	if (prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE)
		return (true);
	if (prev_op == TT_AND && prev_status == 0)
		return (true);
	if (prev_op == TT_OR && prev_status != 0)
		return (true);
	return (false);
}

int execute_simple_list(t_state* state, t_executable_node *exe) {
    int status = 0;
    t_tt op = TT_SEMICOLON;
	t_executable_node exe_curr;
    for (size_t i = 0; i < exe->node->children.len; i++) {
		exe_curr = *exe;
		exe_curr.node = &exe->node->children.buff[i];
		if (should_execute(status, op))
			status = execute_tree_node(state, &exe_curr);
        i++;
        if (i < exe->node->children.len) {
			op = exe->node->children.buff[i].token.tt;
        }
    }
    return status;
}

// returns pid
t_exe_res execute_subshell(t_state *state, t_executable_node *exe)
{
    int pid = fork();
    if (pid == 0) {
		die_on_sig();
		set_up_redirection(state, exe);
		exe->node = &exe->node->children.buff[0];
		exit(execute_tree_node(state, exe));
    }
	return (res_pid(pid));
}

// gives back pid;
t_exe_res execute_command(t_state* state, t_executable_node *exe) {
	assert(exe->node->children.len >= 1);
	if (exe->node->children.buff[0].node_type == AST_SIMPLE_COMMAND) {
		exe->node = &exe->node->children.buff[0];
		return (execute_simple_command(state, exe));
	}
	assert(exe->node->children.buff[0].node_type == AST_SUBSHELL);
	for (size_t i = 1; i < exe->node->children.len; i++) {
		t_ast_node *curr = vec_nd_idx(&exe->node->children, i);
		assert(curr->node_type == AST_REDIRECT);
		int redir_idx;
		if (redirect_from_ast_redir(state, curr, &redir_idx))
		{
			return res_status(AMBIGUOUS_REDIRECT);
		}
		vec_int_push(&exe->redirs, redir_idx);
	}
	exe->node = vec_nd_idx(&exe->node->children, 0);
	exe->modify_parent_context = true;
	return (execute_subshell(state, exe));
}

// always returns status
int execute_tree_node(t_state* state, t_executable_node *exe) {
	int status = 0;
    switch (exe->node->node_type) {
        case AST_COMMAND_PIPELINE:
            status = execute_pipeline(state, exe);
			break;
        case AST_SIMPLE_LIST:
        case AST_COMPOUND_LIST:
            status = execute_simple_list(state, exe);
			break;
        case AST_SIMPLE_COMMAND:
            assert("Unreachable" == 0);
            status = exe_res_to_status(execute_simple_command(state, exe));
			break;
        case AST_COMMAND:
            assert("Unreachable" == 0);
            status = exe_res_to_status(execute_command(state, exe));
			break;
        case AST_SUBSHELL:

        case AST_REDIRECT:
        case AST_WORD:
        case AST_ASSIGNMENT_WORD:
        case AST_TOKEN:
            assert("Unreachable" == 0);
            break;
    }

	free(state->last_cmd_status);
	state->last_cmd_status = ft_itoa(status);
    return (status);
}

void execute_top_level(t_state* state)
{
	t_executable_node exe;

	exe = (t_executable_node){.infd = 0, .outfd = 1, .node = &state->tree, .modify_parent_context = true};
	state->heredoc_idx = 0;
	if (!g_should_unwind)
		gather_heredocs(state, &state->tree);
	if (!g_should_unwind)
		execute_tree_node(state, &exe);
}
