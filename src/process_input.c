/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/23 15:30:54 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <stdbool.h>

#include "libft/libft.h"
#include "libft/dsa/dyn_str.h"

// 1 - EOF
//
// 2 - C-c
int	readline_cmd(t_state *state, char *prompt)
{
	int			stat;

	stat = buff_readline(state, &state->input, prompt);
	free(prompt);
	if (stat == 0 || stat == 2 || !state->input.len)
	{
		if (stat == 0)
		{
			state->should_exit = true;
			return (1);
		}
		if (stat == 2 && state->input_method != INP_READLINE)
		{
			state->should_exit = true;
			return (2);
		}
		return (true);
	}
	return (false);
}

bool	ends_with_bs(t_dyn_str s)
{
	size_t	i;
	bool	unterminated;

	if (s.len == 0)
		return (false);
	i = s.len;
	unterminated = false;
	while (i > 0)
	{
		i--;
		if (s.buff[i] == '\\')
			unterminated = !unterminated;
		else
			break ;
	}
	return (unterminated);
}

// true - EOF
bool	get_more_tokens(t_state *state, char *prompt, t_deque_tt *tt)
{
	int		stat;
	bool	is_bs;

	is_bs = false;
	while (prompt)
	{
		stat = readline_cmd(state, prompt);
		if (stat == 1 && !is_bs)
			return (true);
		else if (stat != 1 && stat)
			break ;
		if (ends_with_bs(state->input))
		{
			dyn_str_pop(&state->input);
			prompt = ft_strdup("> ");
			is_bs = true;
			continue ;
		}
		dyn_str_push(&state->input, '\n');
		prompt = tokenizer(state->input.buff, tt);
		if (prompt)
			prompt = ft_strdup(prompt);
	}
	return (false);
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

	parser = (t_parser){.res = RES_Init};
	prompt = prompt_normal(state).buff;
	deque_tt_init(&tt, 100);
	while (parser.res == RES_MoreInput || parser.res == RES_Init)
	{
		if (get_more_tokens(state, prompt, &tt) && parser.res == RES_MoreInput)
		{
			ft_eprintf("%s: syntax error: unexpected end of file\n",
				state->context);
			set_cmd_status(state, (t_exe_res){.status = SYNTAX_ERR});
		}
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
	state->should_exit |= (g_should_unwind
		&& state->input_method != INP_READLINE) || state->readline_buff.has_finished; // normal exit, don't even try
}
