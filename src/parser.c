/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 00:07:42 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/13 00:21:54 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"

bool	is_simple_cmd_token(t_tt tt)
{
	if (tt == TT_REDIRECT_LEFT
		|| tt == TT_WORD
		|| tt == TT_REDIRECT_RIGHT
		|| tt == TT_APPEND
		|| tt == TT_HEREDOC)
		return (true);
	return (false);
}

void	free_node(t_ast_node *node)
{
	if (node->node_type == AST_TOKEN && node->token.allocated)
		free(node->token.start);
	free(node->children.buff);
	*node = (t_ast_node){};
}

void	free_ast(t_ast_node *node)
{
    ast_postorder_traversal(node, free_node);
}

bool	is_redirect(t_tt tt)
{
	if (tt == TT_REDIRECT_LEFT
		|| tt == TT_REDIRECT_RIGHT
		|| tt == TT_APPEND
		|| tt == TT_HEREDOC)
		return (true);
	return (false);
}

bool	is_compund_list_op(t_tt tt)
{
	if (tt == TT_SEMICOLON
		|| tt == TT_OR
		|| tt == TT_AND
		|| tt == TT_NEWLINE)
		return (true);
	return (false);
}

t_ast_node	parse_pipeline(t_state * state, t_parser *parser, t_deque_tt *tokens);

t_ast_node	unexpected(t_state *state, t_parser *parser, t_ast_node ret, t_deque_tt *tokens)
{
	t_token	t;

	t = deque_tt_peek(tokens);
	ft_printf("%s: syntax error near unexpected token `%.*s'\n",
		state->context, t.len, t.start);
	parser->res = RES_FatalError;
	return (ret);
}

bool	parse_compound_list_s(t_state *state, t_parser *parser, t_deque_tt *tokens, t_ast_node *ret)
{
	t_tt		op;

	op = deque_tt_peek(tokens).tt;
	if (!is_compund_list_op(op))
		return (true);
	vec_nd_push(&ret->children,
		(t_ast_node){.node_type = AST_TOKEN,
		.token = deque_tt_pop_start(tokens)});
	vec_int_push(&parser->parse_stack, op);
	if ((ret->children.buff[ret->children.len - 1].token.tt == TT_SEMICOLON
			|| ret->children.buff[ret->children.len - 1].token.tt == TT_NEWLINE)
		&& deque_tt_peek(tokens).tt == TT_BRACE_RIGHT)
		return (true);
	while (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	if (deque_tt_peek(tokens).tt == TT_END)
	{
		parser->res = RES_MoreInput;
		return (true);
	}
	vec_nd_push(&ret->children, parse_pipeline(state, parser, tokens));
	if (parser->res != RES_OK)
		return (true);
	vec_int_pop(&parser->parse_stack);
	return (false);
}

t_ast_node	parse_compound_list(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;

	ret = (t_ast_node){.node_type = AST_COMPOUND_LIST};
	while (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	if (deque_tt_peek(tokens).tt == TT_END)
		return (parser->res = RES_MoreInput, ret);
	vec_nd_push(&ret.children, parse_pipeline(state, parser, tokens));
	if (parser->res != RES_OK)
		return (ret);
	while (1)
	{
		if (parse_compound_list_s(state, parser, tokens, &ret))
			break ;
	}
	return (ret);
}

t_ast_node	parse_subshell(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;

	ret = (t_ast_node){.node_type = AST_SUBSHELL};
	vec_int_push(&parser->parse_stack, TT_BRACE_LEFT);
	if (deque_tt_peek(tokens).tt != TT_BRACE_LEFT)
		return (unexpected(state, parser, ret, tokens));
	deque_tt_pop_start(tokens);
	vec_nd_push(&ret.children, parse_compound_list(state, parser, tokens));
	if (parser->res != RES_OK)
	{
		return (ret);
	}
	if (deque_tt_peek(tokens).tt != TT_BRACE_RIGHT)
		return (unexpected(state, parser, ret, tokens));
	deque_tt_pop_start(tokens);
	vec_int_pop(&parser->parse_stack);
	return (ret);
}

t_ast_node	parse_word(t_deque_tt *tokens)
{
	t_ast_node	ret;

	ret = (t_ast_node){.node_type = AST_WORD};
	vec_nd_push(&ret.children,
		(t_ast_node){.node_type = AST_TOKEN,
		.token = deque_tt_pop_start(tokens)});
	return (ret);
}

t_ast_node	parse_redirect(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;
	t_token		t;

	ret = (t_ast_node){.node_type = AST_REDIRECT};
	t = deque_tt_pop_start(tokens);
	if (!is_redirect(t.tt))
		return (unexpected(state, parser, ret, tokens));
	vec_nd_push(&ret.children,
		(t_ast_node){.node_type = AST_TOKEN, .token = t});
	if (deque_tt_peek(tokens).tt != TT_WORD)
		return (unexpected(state, parser, ret, tokens));
	vec_nd_push(&ret.children, parse_word(tokens));
	return (ret);
}

t_ast_node	parse_simple_command(t_state *state, t_parser *res, t_deque_tt *tokens)
{
	t_ast_node	ret;
	t_tt		next;

	ret = (t_ast_node){.node_type = AST_SIMPLE_COMMAND};
	next = deque_tt_peek(tokens).tt;
	if (!is_simple_cmd_token(next))
		return (unexpected(state, res, ret, tokens));
	while (1)
	{
		next = deque_tt_peek(tokens).tt;
		if (next == TT_WORD)
			vec_nd_push(&ret.children, parse_word(tokens));
		else if (is_redirect(next))
		{
			vec_nd_push(&ret.children, parse_redirect(state, res, tokens));
			if (res->res != RES_OK)
				return (ret);
		}
		else
			break ;
	}
	return (ret);
}

t_ast_node	parse_command(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;
	t_tt		next;

	ret = (t_ast_node){.node_type = AST_COMMAND};
	next = deque_tt_peek(tokens).tt;
	if (next == TT_BRACE_LEFT)
	{
		vec_nd_push(&ret.children, parse_subshell(state, parser, tokens));
		if (parser->res != RES_OK)
			return (ret);
		while (is_redirect(deque_tt_peek(tokens).tt))
		{
			vec_nd_push(&ret.children, parse_redirect(state, parser, tokens));
			if (parser->res != RES_OK)
				return (ret);
		}
	}
	else
	{
		vec_nd_push(&ret.children, parse_simple_command(state, parser, tokens));
		if (parser->res != RES_OK)
			return (ret);
	}
	return (ret);
}

t_ast_node	parse_pipeline(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;

	ret = (t_ast_node){.node_type = AST_COMMAND_PIPELINE};
	vec_nd_push(&ret.children, parse_command(state, parser, tokens));
	if (parser->res != RES_OK)
		return (ret);
	vec_int_push(&parser->parse_stack, TT_PIPE);
	while (deque_tt_peek(tokens).tt == TT_PIPE)
	{
		deque_tt_pop_start(tokens);
		while (deque_tt_peek(tokens).tt == TT_NEWLINE)
			deque_tt_pop_start(tokens);
		if (deque_tt_peek(tokens).tt == TT_END)
			return (parser->res = RES_MoreInput, ret);
		vec_nd_push(&ret.children, parse_command(state, parser, tokens));
		if (parser->res != RES_OK)
			return (ret);
	}
	vec_int_pop(&parser->parse_stack);
	return (ret);
}

bool	is_simple_list_op(t_tt tt)
{
	if (tt == TT_SEMICOLON
		|| tt == TT_OR
		|| tt == TT_AND)
		return (true);
	return (false);
}

// 0 continue
// 1 break
// 2 return on fail
int	parse_simple_list_s(t_state *state, t_parser *parser, t_deque_tt *tokens, t_ast_node *ret)
{
	t_tt		next;

	next = deque_tt_peek(tokens).tt;
	if (!is_simple_list_op(next))
		return (1);
	vec_int_push(&parser->parse_stack, next);
	vec_nd_push(&ret->children, (t_ast_node){.node_type = AST_TOKEN,
		.token = deque_tt_pop_start(tokens)});
	if (parser->res != RES_OK)
		return (2);
	while (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	if (next == TT_SEMICOLON && deque_tt_peek(tokens).tt == TT_END)
		return (2);
	if (deque_tt_peek(tokens).tt == TT_END)
		return (parser->res = RES_MoreInput, 2);
	vec_nd_push(&ret->children, parse_pipeline(state, parser, tokens));
	if (parser->res != RES_OK)
		return (2);
	vec_int_pop(&parser->parse_stack);
	return (0);
}
t_ast_node	parse_simple_list(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;
	t_tt		next;
	int			status;

	ret = (t_ast_node){.node_type = AST_SIMPLE_LIST};
	next = deque_tt_peek(tokens).tt;
	if (is_simple_cmd_token(next) || next == TT_BRACE_LEFT)
		vec_nd_push(&ret.children, parse_pipeline(state, parser, tokens));
	if (parser->res != RES_OK)
		return (ret);
	while (1)
	{
		status = parse_simple_list_s(state, parser, tokens, &ret);
		if (status == 1)
			break ;
		if (status == 2)
			return (ret);
	}
	if (deque_tt_peek(tokens).tt == TT_NEWLINE)
		deque_tt_pop_start(tokens);
	else
		return (unexpected(state, parser, ret, tokens));
	return (ret);
}

t_ast_node	create_subtoken_node(t_token t, int offset, int end_offset, t_tt tt)
{
	t_ast_node	ret;

	ret = (t_ast_node){.token = {
		.len = end_offset - offset,
		.start = t.start + offset,
		.tt = tt,
	},
		.node_type = AST_TOKEN};
	return (ret);
}

t_ast_node	parse_tokens(t_state* state, t_parser *parser, t_deque_tt *tokens)
{
	t_tt		tt;
	t_ast_node	ret;

	parser->res = RES_OK;
	ret = parse_simple_list(state, parser, tokens);
	if (parser->res == RES_FatalError)
	{
		printf("Parse error!\n");
	}
	else if (parser->res == RES_MoreInput)
	{
		printf("Need more input\n");
	}
	else
	{
		tt = deque_tt_pop_start(tokens).tt;
		if (tt != TT_END)
		{
			printf("Got token: %s\n", tt_to_str(tt));
		}
		assert(tt == TT_END);
		assert(tokens->len == 0);
		reparse_words(&ret);
		reparse_assignment_words(&ret);
	}
	return (ret);
}
