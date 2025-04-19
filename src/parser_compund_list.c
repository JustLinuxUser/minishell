/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_compund_list.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 17:29:37 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 17:36:05 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdbool.h>

bool	is_compund_list_op(t_tt tt)
{
	if (tt == TT_SEMICOLON
		|| tt == TT_OR
		|| tt == TT_AND
		|| tt == TT_NEWLINE)
		return (true);
	return (false);
}

bool	parse_compound_list_s(t_state *state, t_parser *parser,
	t_deque_tt *tokens, t_ast_node *ret)
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

t_ast_node	parse_compound_list(t_state *state,
	t_parser *parser, t_deque_tt *tokens)
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
