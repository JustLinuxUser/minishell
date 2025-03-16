/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/16 06:12:52 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <stdio.h>
#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"

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
					if (should_push)
					{
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
    assert(node->node_type == AST_ASSIGNMENT_WORD);
    assert(node->children.len == 2);

    t_ast_node left = node->children.buff[0];
    t_ast_node right_word = node->children.buff[1];

    assert(right_word.children.len >= 1);
    right_word.children.buff[0].token.len += right_word.children.buff[0].token.start - left.token.start;
    right_word.children.buff[0].token.start = left.token.start;
    free(node->children.buff);
	*node = right_word;
}

int execute_simple_command(t_state* state, t_ast_node node) {
    t_vec_str args;
    bool found_first = false;
    t_vec_env pre_assigns;

    vec_env_init(&pre_assigns);

    vec_str_init(&args);
    t_ast_node *curr;
    for (size_t i = 0; i < node.children.len; i++) {
        curr = &node.children.buff[i];
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
        } else {
            assert("Unimplemented" == 0);
        }
    }

    for (size_t i = 0; i < pre_assigns.len; i++) {
        printf("env: >%s=%s<\n", pre_assigns.buff[i].key, pre_assigns.buff[i].value);
        free(pre_assigns.buff[i].value);
        free(pre_assigns.buff[i].key);
    }
    free(pre_assigns.buff);

    for (size_t i = 0; i < args.len; i++) {
        printf("arg: >%s<\n", args.buff[i]);
        free(args.buff[i]);
    }
    free(args.buff);
    return (0);
}

int execute_tree_node(t_state* state, t_ast_node node) {
    switch (node.node_type) {
        case AST_SIMPLE_COMMAND:
            return execute_simple_command(state, node);
        case AST_SIMPLE_LIST:
        case AST_COMMAND_PIPELINE:
        case AST_AND:
        case AST_OR:
        case AST_SUBSHELL:
        case AST_COMPOUND_LIST:
        case AST_COMMAND:
            assert("Unimplemented" == 0);

        case AST_REDIRECT:
        case AST_WORD:
        case AST_ASSIGNMENT_WORD:
        case AST_TOKEN:
            assert("Unreachable" == 0);
            break;
    }
    return (0);
}

void execute_top_level(t_state* state) {
    execute_tree_node(state, state->tree);
}
