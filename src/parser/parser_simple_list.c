/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_simple_list.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 17:22:54 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 10:20:45 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdbool.h>

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
int	parse_simple_list_s(t_state *state, t_parser *parser,
	t_deque_tt *tokens, t_ast_node *ret)
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

t_ast_node	parse_simple_list(t_state *state,
	t_parser *parser, t_deque_tt *tokens)
{
	t_ast_node	ret;
	t_tt		next;
	int			status;

	ret = (t_ast_node){.node_type = AST_SIMPLE_LIST};
	next = deque_tt_peek(tokens).tt;
	if (!is_simple_cmd_token(next) && next != TT_BRACE_LEFT)
		return (unexpected(state, parser, ret, tokens));
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
	else if (deque_tt_peek(tokens).tt != TT_END)
		return (unexpected(state, parser, ret, tokens));
	return (ret);
}
