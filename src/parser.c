/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 00:07:42 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/18 07:09:47 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "minishell.h"

bool is_simple_cmd_token(t_tt tt) {
    switch (tt) {
        case TT_REDIRECT_LEFT:
        case TT_WORD:
        case TT_REDIRECT_RIGHT:
        case TT_APPEND:
        case TT_HEREDOC:
            return true;
        default:
            break;
    }
    return false;
}

void free_node(t_ast_node* node) {
	if (node->node_type == AST_TOKEN && node->token.allocated)
		free(node->token.start);
    free(node->children.buff);
}

void free_ast(t_ast_node node) {
    ast_postorder_traversal(&node, free_node);
}

bool is_redirect(t_tt tt) {
    switch (tt) {
        case TT_REDIRECT_LEFT:
        case TT_REDIRECT_RIGHT:
        case TT_APPEND:
        case TT_HEREDOC:
            return true;
        default:
            break;
    }
    return (false);
}
bool is_compund_list_op(t_tt t) {
    switch (t) {
        case TT_SEMICOLON:
        case TT_OR:
        case TT_AND:
        case TT_NEWLINE:
            return (true);
        default:
            break;
    }
    return false;
}

t_ast_node parse_pipeline(t_res* res, t_deque_tt* tokens);
t_ast_node unexpected(t_res* res, t_ast_node ret, t_deque_tt* tokens) {
    printf("andriish: syntax error near unexpected token %s\n",
           tt_to_str(deque_tt_peek(tokens).tt));
    *res = R_FatalError;
    assert(false);
    return (ret);
}

t_ast_node parse_compound_list(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMPOUND_LIST};
	while (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	if (deque_tt_peek(tokens).tt == TT_END)
		return (*res = R_MoreInput, ret);
    vec_nd_push(&ret.children, parse_pipeline(res, tokens));
    if (*res != R_OK)
        return (ret);
    while (is_compund_list_op(deque_tt_peek(tokens).tt)) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        if (*res != R_OK)
            return (ret);
        if ((ret.children.buff[ret.children.len - 1].token.tt == TT_SEMICOLON
			|| ret.children.buff[ret.children.len - 1].token.tt == TT_NEWLINE)
			&&
            deque_tt_peek(tokens).tt == TT_BRACE_RIGHT) {
            return (ret);
        }
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (*res = R_MoreInput, ret);
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

t_ast_node parse_subshell(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_SUBSHELL};
    if (deque_tt_peek(tokens).tt != TT_BRACE_LEFT)
        return (unexpected(res, ret, tokens));
    deque_tt_pop_start(tokens);

    vec_nd_push(&ret.children, parse_compound_list(res, tokens));
    if (*res != R_OK) {
        return (ret);
    }
    if (deque_tt_peek(tokens).tt != TT_BRACE_RIGHT)
        return (unexpected(res, ret, tokens));
    deque_tt_pop_start(tokens);
    return (ret);
}

t_ast_node parse_word(t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_WORD};
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN,
                             .token = deque_tt_pop_start(tokens)});
    return (ret);
}

t_ast_node parse_redirect(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_REDIRECT};
    t_token t = deque_tt_pop_start(tokens);

    if (!is_redirect(t.tt)) {
        return (unexpected(res, ret, tokens));
    }
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN, .token = t});
    if (deque_tt_peek(tokens).tt != TT_WORD)
        return (unexpected(res, ret, tokens));
    vec_nd_push(&ret.children, parse_word(tokens));
    return (ret);
}

t_ast_node parse_simple_command(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_SIMPLE_COMMAND};

    t_tt next;
    next = deque_tt_peek(tokens).tt;
    if (!is_simple_cmd_token(next)) {
        return (unexpected(res, ret, tokens));
    }
    while (1) {
        next = deque_tt_peek(tokens).tt;
        if (next == TT_WORD)
            vec_nd_push(&ret.children, parse_word(tokens));
        else if (is_redirect(next)) {
            vec_nd_push(&ret.children, parse_redirect(res, tokens));
            if (*res != R_OK)
                return (ret);
        } else
            break;
    }

    return (ret);
}

t_ast_node parse_command(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_COMMAND};
    t_tt next;
    next = deque_tt_peek(tokens).tt;
    if (next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_subshell(res, tokens));
        if (*res != R_OK)
            return (ret);
        while (is_redirect(deque_tt_peek(tokens).tt)) {
            vec_nd_push(&ret.children, parse_redirect(res, tokens));
            if (*res != R_OK)
                return (ret);
        }
    } else {
        vec_nd_push(&ret.children, parse_simple_command(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

t_ast_node parse_pipeline(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMMAND_PIPELINE};

    vec_nd_push(&ret.children, parse_command(res, tokens));
    if (*res != R_OK)
        return (ret);
    while (deque_tt_peek(tokens).tt == TT_PIPE) {
		deque_tt_pop_start(tokens);
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (*res = R_MoreInput, ret);
        vec_nd_push(&ret.children, parse_command(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

bool is_simple_list_op(t_tt t) {
    switch (t) {
        case TT_SEMICOLON:
        case TT_OR:
        case TT_AND:
            return (true);
        default:
            break;
    }
    return false;
}

t_ast_node parse_simple_list(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_SIMPLE_LIST};

    t_tt next = deque_tt_peek(tokens).tt;
    if (is_simple_cmd_token(next) || next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    while (is_simple_list_op(deque_tt_peek(tokens).tt)) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        if (*res != R_OK)
            return (ret);
        if (ret.children.buff[ret.children.len - 1].token.tt == TT_SEMICOLON &&
            deque_tt_peek(tokens).tt == TT_END) {
            return (ret);
        }
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (*res = R_MoreInput, ret);
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    if (deque_tt_peek(tokens).tt == TT_NEWLINE)
        deque_tt_pop_start(tokens);
    else
        return (unexpected(res, ret, tokens));
    return (ret);
}

bool can_flatten(t_ast_t t) {
    switch (t) {
        break;
        case AST_SIMPLE_LIST:
        case AST_COMMAND_PIPELINE:
        case AST_COMMAND:
        case AST_COMPOUND_LIST:
            return true;
        default:
            break;
    }
    return (false);
}

void flatten(t_ast_node* node) {
    void* temp;

    for (size_t i = 0; i < node->children.len; i++) {
        flatten(&node->children.buff[i]);
    }
    if (node->children.len == 1 && can_flatten(node->node_type)) {
        temp = node->children.buff;
        *node = node->children.buff[0];
        free(temp);
    }
}

t_ast_node create_subtoken_node(t_token t,
                                int offset,
                                int end_offset,
                                t_tt tt) {
    t_ast_node ret;

    ret = (t_ast_node){.token =
                           {
                               .len = end_offset - offset,
                               .start = t.start + offset,
                               .tt = tt,
                           },
                       .node_type = AST_TOKEN};
    return (ret);
}


t_ast_node parse_tokens(t_res* res, t_deque_tt* tokens) {
    *res = R_OK;
    t_ast_node ret = parse_simple_list(res, tokens);
    if (*res == R_FatalError) {
        printf("Parse error!\n");
    } else if (*res == R_MoreInput) {
        printf("Need more input\n");
    } else {
        t_tt tt = deque_tt_pop_start(tokens).tt;
        if (tt != TT_END) {
            printf("Got token: %s\n", tt_to_str(tt));
        }
        assert(tt == TT_END);
        assert(tokens->len == 0);
    }
    // flatten(& ret);
    reparse_words(&ret);
    reparse_assignment_words(&ret);
    return (ret);
}
