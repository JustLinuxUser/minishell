/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 00:07:42 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/21 00:23:54 by anddokhn         ###   ########.fr       */
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

t_ast_node parse_pipeline(t_parser* parser, t_deque_tt* tokens);

t_ast_node unexpected(t_parser* parser, t_ast_node ret, t_deque_tt* tokens) {
    printf("%s: syntax error near unexpected token %s\n",
           parser->prog_name, tt_to_str(deque_tt_peek(tokens).tt));
	parser->res = RES_FatalError;
    return (ret);
}

t_ast_node parse_compound_list(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMPOUND_LIST};
	while (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	if (deque_tt_peek(tokens).tt == TT_END)
		return (parser->res = RES_MoreInput, ret);
    vec_nd_push(&ret.children, parse_pipeline(parser, tokens));
    if (parser->res != RES_OK)
			return (ret);
	t_tt op;
    while (is_compund_list_op(op = deque_tt_peek(tokens).tt)) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
		vec_int_push(&parser->parse_stack, op);
        if ((ret.children.buff[ret.children.len - 1].token.tt == TT_SEMICOLON
			|| ret.children.buff[ret.children.len - 1].token.tt == TT_NEWLINE)
			&&
            deque_tt_peek(tokens).tt == TT_BRACE_RIGHT) {
            return (ret);
        }
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (parser->res = RES_MoreInput, ret);
        vec_nd_push(&ret.children, parse_pipeline(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
		vec_int_pop(&parser->parse_stack);
    }
    return (ret);
}

t_ast_node parse_subshell(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_SUBSHELL};
	vec_int_push(&parser->parse_stack, TT_BRACE_LEFT);
    if (deque_tt_peek(tokens).tt != TT_BRACE_LEFT)
        return (unexpected(parser, ret, tokens));
    deque_tt_pop_start(tokens);

    vec_nd_push(&ret.children, parse_compound_list(parser, tokens));
    if (parser->res != RES_OK) {
        return (ret);
    }
    if (deque_tt_peek(tokens).tt != TT_BRACE_RIGHT)
        return (unexpected(parser, ret, tokens));
    deque_tt_pop_start(tokens);
	vec_int_pop(&parser->parse_stack);
    return (ret);
}

t_ast_node parse_word(t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_WORD};
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN,
                             .token = deque_tt_pop_start(tokens)});
    return (ret);
}

t_ast_node parse_redirect(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_REDIRECT};
    t_token t = deque_tt_pop_start(tokens);

    if (!is_redirect(t.tt)) {
        return (unexpected(parser, ret, tokens));
    }
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN, .token = t});
    if (deque_tt_peek(tokens).tt != TT_WORD)
        return (unexpected(parser, ret, tokens));
    vec_nd_push(&ret.children, parse_word(tokens));
    return (ret);
}

t_ast_node parse_simple_command(t_parser* res, t_deque_tt* tokens) {
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
            if (res->res != RES_OK)
                return (ret);
        } else
            break;
    }

    return (ret);
}

t_ast_node parse_command(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_COMMAND};
    t_tt next;
    next = deque_tt_peek(tokens).tt;
    if (next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_subshell(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
        while (is_redirect(deque_tt_peek(tokens).tt)) {
            vec_nd_push(&ret.children, parse_redirect(parser, tokens));
            if (parser->res != RES_OK)
                return (ret);
        }
    } else {
        vec_nd_push(&ret.children, parse_simple_command(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
    }
    return (ret);
}

t_ast_node parse_pipeline(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMMAND_PIPELINE};

    vec_nd_push(&ret.children, parse_command(parser, tokens));
    if (parser->res != RES_OK)
        return (ret);
	vec_int_push(&parser->parse_stack, TT_PIPE);
    while (deque_tt_peek(tokens).tt == TT_PIPE) {
		deque_tt_pop_start(tokens);
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (parser->res = RES_MoreInput, ret);
        vec_nd_push(&ret.children, parse_command(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
    }
	vec_int_pop(&parser->parse_stack);
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

t_ast_node parse_simple_list(t_parser* parser, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_SIMPLE_LIST};

    t_tt next = deque_tt_peek(tokens).tt;
    if (is_simple_cmd_token(next) || next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_pipeline(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
    }
    while (is_simple_list_op(next = deque_tt_peek(tokens).tt)) {
		vec_int_push(&parser->parse_stack, next);
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        if (parser->res != RES_OK)
            return (ret);
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (next == TT_SEMICOLON && deque_tt_peek(tokens).tt == TT_END) {
            return (ret);
        }
        if (deque_tt_peek(tokens).tt == TT_END)
            return (parser->res = RES_MoreInput, ret);
        vec_nd_push(&ret.children, parse_pipeline(parser, tokens));
        if (parser->res != RES_OK)
            return (ret);
        vec_int_pop(&parser->parse_stack);
    }
    if (deque_tt_peek(tokens).tt == TT_NEWLINE)
        deque_tt_pop_start(tokens);
    else
        return (unexpected(parser, ret, tokens));
    return (ret);
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


t_ast_node parse_tokens(t_parser* parser, t_deque_tt* tokens) {
    parser->res = RES_OK;
    t_ast_node ret = parse_simple_list(parser, tokens);
    if (parser->res == RES_FatalError) {
        printf("Parse error!\n");
    } else if (parser->res == RES_MoreInput) {
        printf("Need more input\n");
    } else {
        t_tt tt = deque_tt_pop_start(tokens).tt;
        if (tt != TT_END) {
            printf("Got token: %s\n", tt_to_str(tt));
        }
        assert(tt == TT_END);
        assert(tokens->len == 0);
		reparse_words(&ret);
		reparse_assignment_words(&ret);
    }
    return (ret);
}
