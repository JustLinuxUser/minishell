/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 13:17:43 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <stdbool.h>
#include <stdio.h>

#include "libft/libft.h"
#include "libft/dsa/dyn_str.h"

// 1 - EOF
//
// 2 - C-c
int	readline_cmd(t_state *state, char **prompt)
{
	int			stat;

	stat = buff_readline(state, &state->input, *prompt);
	free(*prompt);
	*prompt = 0;
	if (stat == 0)
	{
		if (!state->input.len)
		{
			if (state->input_method == INP_READLINE)
				ft_eprintf("exit\n");
			state->should_exit = true;
		}
		return (1);
	}
	if (stat == 2)
	{
		if (state->input_method != INP_READLINE)
			state->should_exit = true;
		return (2);
	}
	return (0);
}

bool	ends_with_bs_nl(t_dyn_str s)
{
	size_t	i;
	bool	unterminated;

	if (s.len == 0)
		return (false);
	i = s.len;
	unterminated = false;
	if (s.buff[--i] != '\n')
		return (false);
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

void	extend_bs(t_state *state)
{
	char	*prompt;

	while (ends_with_bs_nl(state->input))
	{
		dyn_str_pop(&state->input);
		dyn_str_pop(&state->input);
		prompt = ft_strdup("> ");
		if (readline_cmd(state, &prompt))
			return ;
	}
}

void	get_more_tokens(t_state *state, char **prompt, t_deque_tt *tt)
{
	int		stat;

	while (*prompt)
	{
		stat = readline_cmd(state, prompt);
		if (stat)
		{
			if (stat == 1 && state->input.len)
			{
				ft_eprintf("%s: unexpected EOF while"
					" looking for matching `%c'\n",
					state->context, tt->looking_for);
				if (!state->last_cmd_status_res.status)
					set_cmd_status(state, (t_exe_res){.status = SYNTAX_ERR});
				state->should_exit = true;
			}
			return ;
		}
		extend_bs(state);
		*prompt = tokenizer(state->input.buff, tt);
		if (!stat && *prompt)
			*prompt = ft_strdup(*prompt);
	}
}

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
		get_more_tokens(state, &prompt, &tt);
		if (state->should_exit)
			break ;
		if (g_should_unwind)
		{
			set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
			break ;
		}
		if (state->readline_buff.has_finished && parser.res == RES_MoreInput)
		{
			ft_eprintf("%s: syntax error: unexpected end of file\n",
				state->context);
			set_cmd_status(state, (t_exe_res){.status = SYNTAX_ERR});
			break ;
		}
		if (!try_parse_tokens(state, &parser, &tt, &prompt))
			break ;
	}
	if (parser.res == RES_OK)
		execute_tree(state);
	manage_history(state);
	free (parser.parse_stack.buff);
	parser.parse_stack = (t_vec_int){};
	free(tt.buff);
	state->should_exit |= (g_should_unwind
			&& state->input_method != INP_READLINE)
		|| state->readline_buff.has_finished;
}
