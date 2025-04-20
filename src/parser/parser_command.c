/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_command.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 17:35:05 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 22:57:03 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_redirect(t_tt tt)
{
	if (tt == TT_REDIRECT_LEFT
		|| tt == TT_REDIRECT_RIGHT
		|| tt == TT_APPEND
		|| tt == TT_HEREDOC)
		return (true);
	return (false);
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

t_ast_node	parse_simple_command(t_state *state, t_parser *res,
	t_deque_tt *tokens)
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
