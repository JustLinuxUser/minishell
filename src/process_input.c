/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/14 23:53:50 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <stdbool.h>
#include "libft/dsa/dyn_str.h"

bool	is_empty_token_list(t_deque_tt *tokens)
{
	if (tokens->len < 2)
		return (true);
	if (tokens->len == 2
		&& deque_tt_idx(tokens, 0)->tt == TT_NEWLINE)
		return (true);
	return (false);
}

bool	try_parse_tokens(t_state *state, t_parser *parser,
	t_deque_tt *tt, char **prompt)
{
	if (is_empty_token_list(tt))
	{
		buff_readline_reset(&state->readline_buff);
		return (false);
	}
	parser->parse_stack.len = 0;
	state->tree = parse_tokens(state, parser, tt);
	if (parser->res == RES_OK)
		return (true);
	else if (parser->res == RES_MoreInput)
	{
		*prompt = prompt_more_input(parser).buff;
	}
	else if (parser->res == RES_FatalError)
		set_cmd_status(state, (t_exe_res){.status = SYNTAX_ERR});
	free_ast(&state->tree);
	return (true);
}

static void	get_more_input_parser(t_state *state,
			t_parser *parser, char **prompt, t_deque_tt *tt)
{
	while (parser->res == RES_MoreInput || parser->res == RES_Init)
	{
		get_more_tokens(state, prompt, tt);
		if (g_should_unwind)
			set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
		if (state->should_exit || g_should_unwind)
			break ;
		if (!try_parse_tokens(state, parser, tt, prompt))
			break ;
	}
}

void	parse_and_execute_input(t_state *state)
{
	t_deque_tt	tt;
	char		*prompt;
	t_parser	parser;

	parser = (t_parser){.res = RES_Init};
	prompt = prompt_normal(state).buff;
	deque_tt_init(&tt, 100);
	get_more_input_parser(state, &parser, &prompt, &tt);
	if (parser.res == RES_OK)
	{
		execute_top_level(state);
		free_ast(&state->tree);
	}
	if (g_should_unwind)
		set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
	manage_history(state);
	free (parser.parse_stack.buff);
	parser.parse_stack = (t_vec_int){};
	free(tt.buff);
	state->should_exit |= (g_should_unwind
			&& state->input_method != INP_READLINE)
		|| state->readline_buff.has_finished;
}
