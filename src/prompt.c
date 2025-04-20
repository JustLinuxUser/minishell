/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:35:12 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 21:36:13 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "minishell.h"
#include <stdio.h>

#include <readline/readline.h>

t_dyn_str	prompt_more_input(t_parser *parser)
{
	t_dyn_str	ret;
	t_tt		curr;
	size_t		i;

	i = 0;
	dyn_str_init(&ret);
	while (i < parser->parse_stack.len)
	{
		curr = vec_int_idx(&parser->parse_stack, i++);
		if (curr == TT_BRACE_LEFT)
			dyn_str_pushstr(&ret, "subsh");
		else if (curr == TT_PIPE)
			dyn_str_pushstr(&ret, "pipe");
		else if (curr == TT_AND)
			dyn_str_pushstr(&ret, "cmdand");
		else if (curr == TT_OR)
			dyn_str_pushstr(&ret, "cmdor");
		else
			continue ;
		dyn_str_pushstr(&ret, " ");
	}
	ret.buff[ret.len - 1] = '>';
	dyn_str_pushstr(&ret, " ");
	return (ret);
}

t_dyn_str	prompt_normal(t_state *state)
{
	t_dyn_str	ret;

	dyn_str_init(&ret);
	if (state->last_cmd_status_res.status == 0)
		dyn_str_pushstr(&ret, ANSI_GREEN);
	else
	{
		dyn_str_pushstr(&ret, ANSI_RED);
		dyn_str_pushstr(&ret, state->last_cmd_status_s);
		dyn_str_pushstr(&ret, " ");
	}
	dyn_str_pushstr(&ret, PROMPT);
	dyn_str_pushstr(&ret, ANSI_RESET);
	dyn_str_pushstr(&ret, RL_SPACER_1);
	dyn_str_pushstr(&ret, RL_SPACER_1);
	return (ret);
}
