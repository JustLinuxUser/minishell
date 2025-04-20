/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 22:45:22 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "minishell.h"
#include <stdbool.h>

#include "libft/libft.h"
#include "libft/dsa/dyn_str.h"

bool	readline_cmd(t_state *state, char *prompt, t_deque_tt *tt)
{
	int			stat;

	deque_tt_clear(tt);
	stat = buff_readline(state, &state->input, prompt);
	free(prompt);
	if (stat == 0 || stat == 2 || !state->input.len)
	{
		if (stat == 0)
			state->should_exit = true;
		if (stat == 2 && state->input_method != INP_READLINE)
			state->should_exit = true;
		return (true);
	}
	return (false);
}

void	get_more_tokens(t_state *state, char *prompt, t_deque_tt *tt)
{
	while (prompt)
	{
		if (readline_cmd(state, prompt, tt))
			break ;
		if (dyn_str_ends_with_str(&state->input, "\\"))
		{
			dyn_str_pop(&state->input);
			prompt = ft_strdup("> ");
			continue ;
		}
		dyn_str_push(&state->input, '\n');
		prompt = tokenizer(state->input.buff, tt);
		if (prompt)
			prompt = ft_strdup(prompt);
	}
}

bool	try_parse_tokens(t_state *state, t_parser *parser,
	t_deque_tt *tt, char **prompt)
{
	if (!tt->len)
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

void	execute_tree(t_state *state)
{
	execute_top_level(state);
	free_ast(&state->tree);
	if (g_should_unwind)
		set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
}

void	parse_and_execute_input(t_state *state)
{
	t_deque_tt	tt;
	char		*prompt;
	t_parser	parser;

	parser = (t_parser){.res = RES_MoreInput};
	prompt = prompt_normal(state).buff;
	deque_tt_init(&tt, 100);
	while (parser.res == RES_MoreInput)
	{
		get_more_tokens(state, prompt, &tt);
		if (g_should_unwind)
			set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
		if (state->should_exit || g_should_unwind
			|| !try_parse_tokens(state, &parser, &tt, &prompt))
			break ;
	}
	if (parser.res == RES_OK)
		execute_tree(state);
	manage_history(state);
	free (parser.parse_stack.buff);
	parser.parse_stack = (t_vec_int){};
	free(tt.buff);
	state->should_exit |= g_should_unwind
		&& state->input_method != INP_READLINE;
}
