/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/02 16:52:22 by anddokhn         ###   ########.fr       */
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

void expand_word_fully(t_state *state, t_ast_node *node, t_vec_str *args, bool keep_as_one);

void expand_env_vars(t_state* state, t_ast_node *node)
{
    t_token *curr_tt;
    char* temp;

    for (size_t i = 0; i < node->children.len; i++) {
        assert(node->children.buff[i].node_type == AST_TOKEN);
        curr_tt = &node->children.buff[i].token;
        switch (curr_tt->tt) {
            case TT_WORD:
            case TT_SQWORD:
            case TT_DQWORD:
				break;
            case TT_DQENVVAR:
            case TT_ENVVAR:
                temp = env_expand_n(state, curr_tt->start, curr_tt->len);
				curr_tt->start = temp;
				if (curr_tt)
					curr_tt->len = ft_strlen(temp);
				else
					curr_tt->len = 0;
				curr_tt->allocated = false;
                break;
            default:
                assert("Unreachable" == 0);
        }
    }
}

t_ast_node new_env_node(char *new_start)
{
    return (t_ast_node){.node_type = AST_TOKEN,
                        .token = {.allocated = true,
                                  .len = ft_strlen(new_start),
                                  .start = new_start,
                                  .tt = TT_ENVVAR}};
}

t_vec_nd split_words(t_ast_node *node) {
	t_vec_nd ret = {};
    t_token *curr_tt;
	char** things;

    t_ast_node curr_node = {.node_type = AST_WORD};

    for (size_t i = 0; i < node->children.len; i++) {
        assert(node->children.buff[i].node_type == AST_TOKEN);
        curr_tt = &node->children.buff[i].token;
        switch (curr_tt->tt) {
            case TT_WORD:
            case TT_SQWORD:
            case TT_DQWORD:
            case TT_DQENVVAR:
				vec_nd_push(&curr_node.children, node->children.buff[i]);
				break;
            case TT_ENVVAR:
				if (!curr_tt->start)
					break;
				// IFS
                things = ft_split(curr_tt->start, ' ');
				// free(curr_tt->start);
                if (things[0])
				{
					vec_nd_push(&curr_node.children, new_env_node(things[0]));
                    int i = 1;
                    for (; things[i]; i++)
					{
						vec_nd_push(&ret, curr_node);
						curr_node = (t_ast_node){.node_type = AST_WORD};
						vec_nd_push(&curr_node.children, new_env_node(things[i]));
                    }
                }
				else if (curr_node.children.len)
				{
					vec_nd_push(&ret, curr_node);
					curr_node = (t_ast_node){.node_type = AST_WORD};
                }
				free(things);
                break;
            default:
                assert("Unreachable" == 0);
        }
    }
	if (curr_node.children.len)
		vec_nd_push(&ret, curr_node);
	return (ret);
}

t_dyn_str word_to_string(t_ast_node node) {
    t_token curr;
    t_dyn_str s;
    dyn_str_init(&s);

    for (size_t i = 0; i < node.children.len; i++) {
        assert(node.children.buff[i].node_type == AST_TOKEN);
        curr = node.children.buff[i].token;
        switch (curr.tt) {
            case TT_WORD:
            case TT_SQWORD:
            case TT_DQWORD:
            case TT_DQENVVAR:
            case TT_ENVVAR:
                dyn_str_pushnstr(&s, curr.start, curr.len);
				break;
            default:
                assert("Unreachable" == 0);
        }
    }
	return (s);
}

t_env assignment_to_env(t_state* state, t_ast_node node) {
    t_vec_str args;
    t_env ret = {};
    vec_str_init(&args);
    assert(node.children.len == 2);

    expand_word_fully(state, &node.children.buff[1], &args, true);

    ret.key = ft_strndup(node.children.buff[0].token.start,
                         node.children.buff[0].token.len);
    if (args.len) {
        assert(args.len == 1);
        ret.value = args.buff[0];
    }
    free(args.buff);
    return (ret);
}

void assignment_word_to_word(t_ast_node* node) {
    t_ast_node ret = {.node_type = AST_WORD};

    assert(node->node_type == AST_ASSIGNMENT_WORD);
    assert(node->children.len == 2);

    t_ast_node left = node->children.buff[0];
    t_ast_node right_word = node->children.buff[1];

    left.token.len++;
    vec_nd_push(&ret.children, left);
    vec_nd_push_vec(&ret.children, &right_word.children);
    free(right_word.children.buff);
    free(node->children.buff);
    *node = ret;
}

bool token_starts_with(t_token t, char* str) {
    int i;

    i = 0;
    while (str[i] && i < t.len) {
        if (str[i] != t.start[i])
            return (false);
        i++;
    }
    return (true);
}

void expand_tilde_token(t_state* state, t_token* t) {
    // 0 norm
    // 1 +/-
    int template_len;
    char* env_val;
    if (token_starts_with(*t, "~+")) {
        env_val = env_expand(state, "PWD");
        if (!env_val)
            return;
        template_len = 2;
    } else if (token_starts_with(*t, "~-")) {
        env_val = env_expand(state, "OLDPWD");
        if (!env_val)
            return;
        template_len = 2;
    } else {
        env_val = env_expand(state, "HOME");
        if (!env_val)
            return;
        template_len = 1;
    }
    t_dyn_str s;
    dyn_str_init(&s);

    if (env_val)
        dyn_str_pushstr(&s, env_val);
    t->allocated = true;
    dyn_str_pushnstr(&s, t->start + template_len, t->len - template_len);
    t->start = s.buff;
    t->len = s.len;
}

void expand_tilde_word(t_state* state, t_ast_node* curr) {
    assert(curr->children.len);
    if (curr->children.buff[0].token.tt != TT_WORD)
        return;
    t_token* first = &curr->children.buff[0].token;
    bool should_expand = false;
    should_expand |= token_starts_with(*first, "~") &&
                     curr->children.len == 1 && first->len == 1;
    should_expand |= token_starts_with(*first, "~/");
    should_expand |= token_starts_with(*first, "~+") &&
                     curr->children.len == 1 && first->len == 2;
    should_expand |= token_starts_with(*first, "~+/");
    should_expand |= token_starts_with(*first, "~-") &&
                     curr->children.len == 1 && first->len == 2;
    should_expand |= token_starts_with(*first, "~-/");
    if (should_expand) {
        expand_tilde_token(state, first);
    }
}

void tilde_expand_simple_command(t_state* state, t_ast_node* node) {
    t_ast_node* curr;

    for (size_t i = 0; i < node->children.len; i++) {
        curr = &node->children.buff[i];
        if (curr->node_type == AST_WORD) {
            expand_tilde_word(state, curr);
        } else if (curr->node_type == AST_ASSIGNMENT_WORD) {
            expand_tilde_word(state, &curr->children.buff[1]);
        } else if (curr->node_type == AST_REDIRECT) {
            t_tt redir_type = curr->children.buff[0].token.tt;
            if (redir_type == TT_REDIRECT_LEFT ||
                redir_type == TT_REDIRECT_RIGHT || redir_type == TT_APPEND) {
                expand_tilde_word(state, &curr->children.buff[1]);
            }
        } else {
            assert("Unreachable!!" == 0);
        }
    }
}

char* exe_path(char** path_dirs, char* exe_name) {
    t_dyn_str temp;
    int i;

    if (ft_strchr(exe_name, '/') || ft_strlen(exe_name) == 0)
        return (ft_strdup(exe_name));
    dyn_str_init(&temp);
    i = -1;
    while (path_dirs[++i]) {
        dyn_str_clear(&temp);
        dyn_str_pushstr(&temp, path_dirs[i]);
        dyn_str_push(&temp, '/');
        dyn_str_pushstr(&temp, exe_name);

        if (access(temp.buff, F_OK) == 0)
            break;
        else if (path_dirs[i + 1] == 0)
            return (free(temp.buff), NULL);
    }
    if (temp.len > 0)
        return (temp.buff);
    return (free(temp.buff), NULL);
}

void err_cmd_not_found(t_state* state, char* cmd) {
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
		free_all_state(*state);
        return (COMMAND_NOT_FOUND);
    }
    char** path_dirs = ft_split(path, ':');
    char* path_of_exe = exe_path(path_dirs, args->buff[0]);

    free_tab(path_dirs);
    if (!path_of_exe) {
        err_cmd_not_found(state, args->buff[0]);
		free_all_state(*state);
        return (COMMAND_NOT_FOUND);
    }
    vec_str_push(args, 0);  // nullterm
    char** envp = get_envp(state);
    execve(path_of_exe, args->buff, envp);
	free_all_state(*state);
	// err_cmd_other(state, args->buff[0]);
	// free_tab(args->buff);
	// free_tab(envp);
	// free_ast(state->tree);
	free(path_of_exe);
    return (EXE_PERM_DENIED);
}

char* expand_word_single(t_state* state, t_ast_node* curr)
{
    t_vec_str args = {};
    expand_word_fully(state, curr, &args, false);
    if (args.len != 1) {
        for (size_t i = 0; i < args.len; i++)
            free(args.buff[i]);
        free(args.buff);
        return (0);
    }
    char* temp = args.buff[0];
    free(args.buff);
    return (temp);
}

int redirect_from_ast_redir(t_state *state, t_ast_node *curr, int *redir_idx)
{
	t_redir ret;
	assert(curr->node_type == AST_REDIRECT);
	if (curr->has_redirect)
	{
		ret = state->redirects.buff[curr->redir_idx];
		return (0);
	}
	t_tt tt = curr->children.buff[0].token.tt;
	assert(tt != TT_HEREDOC && "Unimplemented heredocs");
	ret.direction_in = tt == TT_REDIRECT_LEFT || tt == TT_HEREDOC;

	ret.fname = expand_word_single(state, vec_nd_idx(&curr->children, 1));
	if (!ret.fname) {
		return (-1);  // TODO: Get the proper exit code
	}
	if (tt == TT_REDIRECT_LEFT)
		ret.fd = open(ret.fname, O_RDONLY);
	else if (tt == TT_REDIRECT_RIGHT)
		ret.fd =
			open(ret.fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else if (tt == TT_APPEND)
		ret.fd =
			open(ret.fname, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (ret.fd < 0)
		return (-1);  // TODO: Get the proper exit code
	ret.should_delete = false;
	curr->has_redirect = true;
	vec_redir_push(&state->redirects, ret);
	curr->redir_idx = state->redirects.len - 1;
	*redir_idx = state->redirects.len - 1;
	return (0);
}

void expand_word_fully(t_state *state, t_ast_node *node, t_vec_str *args, bool keep_as_one)
{
	t_vec_nd	words;
	t_vec_str	glob_words;
	t_dyn_str 	temp;

	expand_env_vars(state, node);
	vec_nd_init(&words);
	if (!keep_as_one)
		words = split_words(node);
	else
		vec_nd_push(&words, *node);
	for (size_t i = 0; i < words.len; i++) {
		glob_words = expand_word_glob(words.buff[i]);
		dyn_str_init(&temp);
		for (size_t i = 0; i < glob_words.len; i++)
		{
			if (!keep_as_one)
				vec_str_push(args, glob_words.buff[i]);
			else
				dyn_str_pushstr(&temp, glob_words.buff[i]);
			if (keep_as_one && i + 1 < glob_words.len)
				dyn_str_push(&temp, ' ');
		}
		if (keep_as_one)
			vec_str_push(args, temp.buff);
		free(glob_words.buff);
		free_ast(words.buff[i]);
	}
	free(words.buff);
}

bool	is_export(t_ast_node word)
{
	t_ast_node	c;
	if (word.children.len != 1)
		return (false);
	c = word.children.buff[0];
	if (c.token.tt != TT_WORD)
		return false;
	if (ft_strncmp(c.token.start, "export", c.token.len))
		return (false);
	return (true);
}

int expand_simple_command(t_state* state, t_ast_node* node, executable_cmd_t *ret, t_vec_int * redirects) {
	 *ret = (executable_cmd_t){};

    bool found_first = false;
	bool export = false;

    vec_str_init(&ret->argv);
    tilde_expand_simple_command(state, node);

    t_ast_node* curr;
    for (size_t i = 0; i < node->children.len; i++) {
        curr = &node->children.buff[i];
        if (curr->node_type == AST_WORD) {
			expand_word_fully(state, curr, &ret->argv, false);
			if (!found_first && is_export(*curr))
				export = true;
            found_first = true;
        } else if (curr->node_type == AST_ASSIGNMENT_WORD) {
            if (!found_first)
			{
                vec_env_push(&ret->pre_assigns, assignment_to_env(state, *curr));
            }
			else
			{
                assignment_word_to_word(curr);
                print_node(*curr);
				if (export)
					expand_word_fully(state, curr, &ret->argv, true);
				else
					expand_word_fully(state, curr, &ret->argv, false);
            }
        } else if (curr->node_type == AST_REDIRECT) {
			int redir_idx;
			if (redirect_from_ast_redir(state, curr, &redir_idx)) {
				ft_eprintf("Error expanding redirect!!\n");
				return (1);
			}
			vec_int_push(redirects, redir_idx);
        } else {
            assert("Unimplemented" == 0);
        }
    }
	return (0);
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

void free_executable_node(t_state *state, t_executable_node node)
{
	t_redir	*curr;

    for (size_t i = 0; i < node.redirs.len; i++) {
		printf("i: %i\n", node.redirs.buff[i]);
		printf("len: %zu\n", state->redirects.len);
		curr = &state->redirects.buff[node.redirs.buff[i]];
		if (curr->should_delete)
			unlink(curr->fname);
        free(curr->fname);
		ft_memset(curr, 0, sizeof(*curr));
    }
    free(node.redirs.buff);
}

void set_up_redirection(t_state *state, t_executable_node exe)
{
	t_redir redir;

	if (exe.next_infd)
		close(exe.next_infd);
	if (exe.outfd != 1)
	{
		dup2(exe.outfd, 1);
		close(exe.outfd);
	}
	if (exe.infd != 0)
	{
		dup2(exe.infd, 0);
		close(exe.infd);
	}
	for (size_t i = 0; i < exe.redirs.len; i++) {
		redir = state->redirects.buff[exe.redirs.buff[i]];
		if (redir.direction_in) {
			dup2(redir.fd, 0);
		} else {
			dup2(redir.fd, 1);
		}
		close(redir.fd);
	}
}

// returns the PID if any, and -1 if none
t_exe_res execute_simple_command(t_state* state,
                           t_executable_node exe) {
	executable_cmd_t cmd;
	if (expand_simple_command(state, exe.node, &cmd, &exe.redirs))
	{
		// TODO: Fix this
		ft_eprintf("Failed to expand the command!\n");
		return (res_status(AMBIGUOUS_REDIRECT));
	}
    for (size_t i = 0; i < cmd.pre_assigns.len; i++) {
        printf("env: >%s=%s<\n", cmd.pre_assigns.buff[i].key,
               cmd.pre_assigns.buff[i].value);
    }

    for (size_t i = 0; i < cmd.argv.len; i++) {
        printf("arg: >%s<\n", cmd.argv.buff[i]);
    }


	if (builtin_func(cmd.argv.buff[0]) && exe.modify_parent_context) {
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

    int pid = fork();
    if (pid == 0) {
		set_up_redirection(state, exe);
        env_extend(&state->env, &cmd.pre_assigns);
        exit(actually_run(state, &cmd.argv));
    }
	free_executable_cmd(cmd);
	free_executable_node(state, exe);
	return (res_pid(pid));
}

// Always returns status
int execute_pipeline(t_state* state, t_executable_node exe) {
	t_executable_node curr_exe = {};

    curr_exe.infd = dup(exe.infd);
    // curr_exe.outfd = dup(exe.outfd);
	curr_exe.modify_parent_context = exe.node->children.len == 1; // if its just one, it's ok

    int pp[2];
    size_t i = 0;
    t_vec_exe_res results = {};
    t_exe_res res;

	for (; i < exe.node->children.len - 1; i++) {
		curr_exe.node = vec_nd_idx(&exe.node->children, i);
		if (curr_exe.node->node_type == AST_COMMAND) {
			pipe(pp);
			curr_exe.outfd = pp[1];
			curr_exe.next_infd = pp[0];
			res = execute_command(state, curr_exe);
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
	if (exe.node->children.buff[i].node_type == AST_COMMAND) {
		curr_exe.node = vec_nd_idx(&exe.node->children, i);
		curr_exe.next_infd = 0;
		curr_exe.outfd = dup(exe.outfd);
		res = execute_command(state, curr_exe);
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

int execute_tree_node(t_state* state, t_executable_node exe);

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

int execute_simple_list(t_state* state, t_executable_node exe) {
    int status = 0;
    t_tt op = TT_SEMICOLON;
	t_executable_node exe_curr;
    for (size_t i = 0; i < exe.node->children.len; i++) {
		exe_curr = exe;
		exe_curr.node = &exe.node->children.buff[i];
		if (should_execute(status, op))
			status = execute_tree_node(state, exe_curr);
        i++;
        if (i < exe.node->children.len) {
			op = exe.node->children.buff[i].token.tt;
        }
    }
    return status;
}

// returns pid
t_exe_res execute_subshell(t_state *state, t_executable_node exe)
{
    int pid = fork();
    if (pid == 0) {
		set_up_redirection(state, exe);
		exe.node = &exe.node->children.buff[0];
		exit(execute_tree_node(state, exe));
    }
	return (res_pid(pid));
}

// gives back pid;
t_exe_res execute_command(t_state* state, t_executable_node exe) {
	assert(exe.node->children.len >= 1);
	if (exe.node->children.buff[0].node_type == AST_SIMPLE_COMMAND) {
		exe.node = &exe.node->children.buff[0];
		return (execute_simple_command(state, exe));
	}
	assert(exe.node->children.buff[0].node_type == AST_SUBSHELL);
	for (size_t i = 1; i < exe.node->children.len; i++) {
		t_ast_node *curr = vec_nd_idx(&exe.node->children, i);
		assert(curr->node_type == AST_REDIRECT);
		int redir_idx;
		if (redirect_from_ast_redir(state, curr, &redir_idx))
		{
			ft_eprintf("Failed to expand redir\n");
			return res_status(1);// TODO: Get the proper exit code
		}
		vec_int_push(&exe.redirs, redir_idx);
	}
	exe.node = vec_nd_idx(&exe.node->children, 0);
	exe.modify_parent_context = true;
	return (execute_subshell(state, exe));
}

// always returns status
int execute_tree_node(t_state* state, t_executable_node exe) {
	int status = 0;
    switch (exe.node->node_type) {
        case AST_SIMPLE_COMMAND:
            status =  exe_res_to_status(execute_simple_command(state, exe));
			break;
        case AST_COMMAND_PIPELINE:
            status = execute_pipeline(state, exe);
			break;
        case AST_SIMPLE_LIST:
        case AST_COMPOUND_LIST:
            status = execute_simple_list(state, exe);
			break;
        case AST_COMMAND:
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

void execute_top_level(t_state* state) {
	t_executable_node exe;

	exe = (t_executable_node){.infd = 0, .outfd = 1, .node = &state->tree, .modify_parent_context = true};
	state->heredoc_idx = 0;
	if (!should_unwind)
		gather_heredocs(state, &state->tree);
	if (!should_unwind)
		execute_tree_node(state, exe);
}
