/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_more_tokens.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:43:56 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/08 19:51:46 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <stdbool.h>

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
		if (stat == 1)
		{
			if (tt->looking_for && state->input.len)
				ft_eprintf("%s: unexpected EOF while looking for "
					"matching `%c'\n", state->context, tt->looking_for);
			else if (state->input.len)
				ft_eprintf("%s: syntax error: unexpected end of file\n",
					state->context);
			if (state->input_method == INP_READLINE)
				ft_eprintf("exit\n");
			if (!state->last_cmd_status_res.status && state->input.len)
				set_cmd_status(state, (t_exe_res){.status = SYNTAX_ERR});
			state->should_exit = true;
		}
		if (stat)
			return ;
		*prompt = (extend_bs(state), tokenizer(state->input.buff, tt));
		if (*prompt)
			*prompt = ft_strdup(*prompt);
	}
}
