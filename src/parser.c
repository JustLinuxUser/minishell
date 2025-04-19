/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 00:07:42 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 17:35:33 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <assert.h>
#include <stdio.h>
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

t_ast_node	parse_tokens(t_state *state, t_parser *parser, t_deque_tt *tokens)
{
	t_tt		tt;
	t_ast_node	ret;

	parser->res = RES_OK;
	ret = parse_simple_list(state, parser, tokens);
	if (parser->res == RES_OK)
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
