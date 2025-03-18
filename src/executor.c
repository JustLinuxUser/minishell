/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/18 05:36:15 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/deque_int.h"
#include "minishell.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dsa/vec_env.h"
#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"

void expand_word(t_state* state,
                 t_ast_node node,
                 t_vec_str* args,
                 bool keep_as_one) {
    t_token curr;
    t_dyn_str s;
    t_env* temp;
    bool should_push = false;
    dyn_str_init(&s);

    for (size_t i = 0; i < node.children.len; i++) {
        assert(node.children.buff[i].node_type == AST_TOKEN);
        curr = node.children.buff[i].token;
        switch (curr.tt) {
            case TT_WORD:
                dyn_str_pushnstr(&s, curr.start, curr.len);
                should_push = true;
                break;
            case TT_SQWORD:
                dyn_str_pushnstr(&s, curr.start, curr.len);
                should_push = true;
                break;
            case TT_DQWORD:
                dyn_str_pushnstr(&s, curr.start, curr.len);
                should_push = true;
                break;
            case TT_DQENVVAR:
                temp = env_nget(&state->env, curr.start, curr.len);
                if (temp && temp->value)
                    dyn_str_pushstr(&s, temp->value);
                should_push = true;
                break;
            case TT_ENVVAR:
                temp = env_nget(&state->env, curr.start, curr.len);
                if (!temp || !temp->value)
                    break;
                if (keep_as_one) {
                    dyn_str_pushstr(&s, temp->value);
                    should_push = true;
                    break;
                }
                char** things = ft_split(temp->value, ' ');
                if (things[0]) {
                    dyn_str_pushstr(&s, things[0]);
                    should_push = true;
                    int i = 1;
                    for (; things[i]; i++) {
                        vec_str_push(args, s.buff);
                        dyn_str_init(&s);
                        dyn_str_pushstr(&s, things[i]);
                    }
                } else {
                    if (should_push) {
                        vec_str_push(args, s.buff);
                        dyn_str_init(&s);
                    }
                    should_push = false;
                }
                for (int i = 0; things[i]; i++)
                    free(things[i]);
                free(things);
                break;
            default:
                assert("Unreachable" == 0);
        }
    }
    if (should_push)
        vec_str_push(args, s.buff);
    else
        free(s.buff);
}

t_env assignment_to_env(t_state* state, t_ast_node node) {
    t_vec_str args;
    t_env ret = {};
    vec_str_init(&args);
    assert(node.children.len == 2);

    expand_word(state, node.children.buff[1], &args, true);

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
    t_env* env;
    if (token_starts_with(*t, "~+")) {
        env = env_get(&state->env, "PWD");
        if (!env)
            return;
        template_len = 2;
    } else if (token_starts_with(*t, "~-")) {
        env = env_get(&state->env, "OLDPWD");
        if (!env)
            return;
        template_len = 2;
    } else {
        env = env_get(&state->env, "HOME");
        if (!env)
            return;
        template_len = 1;
    }
    t_dyn_str s;
    dyn_str_init(&s);

    if (env->value)
        dyn_str_pushstr(&s, env->value);
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
            printf("Unimplemented node: \n");
            print_node(*curr);
            printf("\n");
            assert("Unimplemented" == 0);
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

struct new_cmd_run_s {
    bool is_buitin;
    int in_fd;
    int out_fd;
    t_vec_str envp;
    t_vec_str args;
    char* exe_path;
} new_cmd_run_t;

void cmd_not_found(t_state* state, char* cmd) {
    ft_eprintf("%s: %s: command not found\n", state->argv[0], cmd);
}

void free_tab(char** tab) {
    int i;
    i = 0;

    while (tab[i])
        free(tab[i++]);
    free(tab);
}

int actually_run(t_state* state, t_vec_str* args) {
    assert(args->len >= 1);
    ft_eprintf("executing cmd: %s\n", args->buff[0]);
    t_env* path = env_get(&state->env, "PATH");
    if (!path || !path->value) {
        ft_eprintf("No path found");
        cmd_not_found(state, args->buff[0]);
        return (-1);  // TODO: should be replaced with a valid code, and made a
                      // define for it
    }
    char** path_dirs = ft_split(path->value, ':');
    char* path_of_exe = exe_path(path_dirs, args->buff[0]);

    free_tab(path_dirs);
    if (!path_of_exe) {
        ft_eprintf("Not found in path\n");
        cmd_not_found(state, args->buff[0]);
        return (-1);  // TODO: should be replaced with a valid code, and made a
                      // define for it
    }
    vec_str_push(args, 0);  // nullterm
    char** envp = env_to_envp(&state->env);
    execve(path_of_exe, args->buff, envp);
    ft_eprintf("ERROR: %s\n", strerror(errno));
    return (0);
}

char* expand_word_single(t_state* state, t_ast_node* curr) {
    t_vec_str args = {};
    expand_word(state, *curr, &args, false);
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

// returns the PID if any, and -1 if none
int execute_simple_command(t_state* state,
                           t_ast_node* node,
                           int infd,
                           int outfd) {
    t_vec_str args;
    bool found_first = false;
    t_vec_env pre_assigns;
    t_vec_redir redirects = {};

    vec_env_init(&pre_assigns);
    vec_str_init(&args);
    tilde_expand_simple_command(state, node);
    t_ast_node* curr;
    for (size_t i = 0; i < node->children.len; i++) {
        curr = &node->children.buff[i];
        if (curr->node_type == AST_WORD) {
            expand_word(state, *curr, &args, false);
            found_first = true;
        } else if (curr->node_type == AST_ASSIGNMENT_WORD) {
            if (!found_first) {
                vec_env_push(&pre_assigns, assignment_to_env(state, *curr));
            } else {
                assignment_word_to_word(curr);
                print_node(*curr);
                expand_word(state, *curr, &args, false);
            }
        } else if (curr->node_type == AST_REDIRECT) {
            redir_t redir = {};
            t_tt tt = curr->children.buff[0].token.tt;
            assert(tt != TT_HEREDOC && "Unimplemented heredocs");
            redir.direction_in = tt == TT_REDIRECT_LEFT || tt == TT_HEREDOC;

            redir.fname =
                expand_word_single(state, vec_nd_idx(&curr->children, 1));
            if (!redir.fname) {
                return (-1);  // TODO: Get the proper exit code
            }
            if (tt == TT_REDIRECT_LEFT)
                redir.fd = open(redir.fname, O_RDONLY);
            else if (tt == TT_REDIRECT_RIGHT)
                redir.fd =
                    open(redir.fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            else if (tt == TT_APPEND)
                redir.fd =
                    open(redir.fname, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (redir.fd < 0)
                return (-1);  // TODO: Get the proper exit code
            redir.should_delete = false;
            vec_redir_push(&redirects, redir);
        } else {
            assert("Unimplemented" == 0);
        }
    }

    for (size_t i = 0; i < pre_assigns.len; i++) {
        printf("env: >%s=%s<\n", pre_assigns.buff[i].key,
               pre_assigns.buff[i].value);
    }

    for (size_t i = 0; i < args.len; i++) {
        printf("arg: >%s<\n", args.buff[i]);
    }

    int pid = fork();
    if (pid == 0) {
        dup2(infd, 0);
        dup2(outfd, 1);
        if (infd != 0)
            close(infd);
        if (outfd != 1)
            close(outfd);
        for (size_t i = 0; i < redirects.len; i++) {
            if (redirects.buff[i].direction_in) {
                dup2(redirects.buff[i].fd, 0);
            } else {
                dup2(redirects.buff[i].fd, 1);
            }
        }
        env_extend(&state->env, &pre_assigns);
        actually_run(state, &args);
    }
    for (size_t i = 0; i < pre_assigns.len; i++) {
        free(pre_assigns.buff[i].value);
        free(pre_assigns.buff[i].key);
    }

    for (size_t i = 0; i < args.len; i++) {
        free(args.buff[i]);
    }
    free(pre_assigns.buff);
    free(args.buff);

    for (size_t i = 0; i < redirects.len; i++) {
        free(redirects.buff[i].fname);
    }
    free(redirects.buff);
    return (pid);
}

int execute_pipeline(t_state* state, t_ast_node* node, int infd, int outfd) {
    int curr_in = infd;
    int curr_out;
    int pp[2];
    size_t i = 0;
    t_deque_int pids;

    deque_int_init(&pids, 10);

    int pid;

    for (; i < node->children.len - 1; i++) {
        if (node->children.buff[i].node_type == AST_SIMPLE_COMMAND) {
            pipe(pp);
            curr_out = pp[1];
            pid = execute_simple_command(state, &node->children.buff[i],
                                         curr_in, curr_out);
            deque_int_push_end(&pids, pid);
            close(curr_out);
            ft_eprintf("curr_in: %i\n", curr_in);
            close(curr_in);
            curr_in = pp[0];
            ft_eprintf("curr_in: %i\n", curr_in);
        } else {
            assert("Unimplemented" == 0);
        }
    }
    if (node->children.buff[i].node_type == AST_SIMPLE_COMMAND) {
        pid = execute_simple_command(state, &node->children.buff[i], curr_in,
                                     outfd);
        deque_int_push_end(&pids, pid);
        // close(curr_out);
        // close(outfd);
    } else {
        assert("Unimplemented" == 0);
    }
    int status;
    while (pids.len) {
        pid = deque_int_pop_start(&pids);
        assert(pid > 0);
        waitpid(pid, &status, 0);
    }
    free(pids.buff);
    return (status);
    return (0);
}

int execute_tree_node(t_state* state, t_ast_node* node);

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

int execute_simple_list(t_state* state, t_ast_node* node) {
    int status = 0;
    t_tt op = TT_SEMICOLON;
    for (size_t i = 0; i < node->children.len; i++) {
		if (should_execute(status, op))
			status = execute_tree_node(state, &node->children.buff[i]);
        i++;
        if (i < node->children.len) {
			op = node->children.buff[i].token.tt;
        }
    }
    return status;
}

int execute_tree_node(t_state* state, t_ast_node* node) {
    int status;
    int pid;

    switch (node->node_type) {
        case AST_SIMPLE_COMMAND:
            pid = execute_simple_command(state, node, 0, 1);
            waitpid(pid, &status, 0);
            return (status);
        case AST_COMMAND_PIPELINE:
            return execute_pipeline(state, node, 0, 1);
        case AST_SIMPLE_LIST:
        case AST_COMPOUND_LIST:
            return execute_simple_list(state, node);
        case AST_COMMAND:
            assert("Unimplemented" == 0);
        case AST_SUBSHELL:

        case AST_REDIRECT:
        case AST_WORD:
        case AST_ASSIGNMENT_WORD:
        case AST_TOKEN:
            assert("Unreachable" == 0);
            break;
    }
    return (0);
}

int gather_heredocs(t_state* state, t_ast_node* node) {
    size_t i;

    i = 0;
    while (i < node->children.len) {
        gather_heredocs(state, &node->children.buff[i]);
    }
    if (node->node_type == AST_REDIRECT) {
        assert(node->children.len >= 1);
        if (node->children.buff[0].token.tt == TT_HEREDOC) {
        }
    }
    return (0);
}

void execute_top_level(t_state* state) {
    execute_tree_node(state, &state->tree);
}
