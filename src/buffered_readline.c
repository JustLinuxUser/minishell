/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffered_readline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:23:53 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/06 10:28:45 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "minishell.h"
#include "libft/libft.h"
#include <readline/readline.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>

int	g_should_unwind = 0;

int	return_last_line(t_state *state, t_dyn_str *ret)
{
	int		len;

	len = state->readline_buff.buff.len - state->readline_buff.cursor;
	dyn_str_pushnstr(ret, state->readline_buff.buff.buff
		+ state->readline_buff.cursor, len);
	state->readline_buff.cursor = 0;
	state->readline_buff.buff.len = 0;
	state->readline_buff.has_line = false;
	if (len == 0)
		return (1);
	return (4);
}

int	return_new_line(t_state *state, t_dyn_str *ret)
{
	char	*temp;
	int		len;

	state->readline_buff.line++;
	update_context(state);
	temp = ft_strchr(state->readline_buff.buff.buff
			+ state->readline_buff.cursor, '\n');
	if (temp == 0)
		return (return_last_line(state, ret));
	len = temp - (state->readline_buff.buff.buff
			+ state->readline_buff.cursor) + 1;
	if (len)
	{
		dyn_str_pushnstr(ret, state->readline_buff.buff.buff
			+ state->readline_buff.cursor, len);
	}
	state->readline_buff.cursor += len;
	state->readline_buff.has_line = state->readline_buff.cursor
		!= state->readline_buff.buff.len;
	if (len == 0)
		return (1);
	return (4);
}

int	buff_readline(t_state *state, t_dyn_str *ret, char *prompt)
{
	int		code;

	if (state->readline_buff.has_finished)
		return (0);
	if (!state->readline_buff.has_line)
	{
		if (state->input_method == INP_ARG || state->input_method == INP_FILE)
			return (state->readline_buff.has_finished = true, 0);
		if (state->input_method == INP_STDIN_NOTTY)
			code = get_more_input_notty(state);
		else
			code = get_more_input_readline(&state->readline_buff, prompt);
		if (code == 1)
			return (state->readline_buff.has_finished = true, 0);
		if (code == 2)
		{
			g_should_unwind = 1;
			set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
			return (2);
		}
		if (state->input_method == INP_READLINE)
			dyn_str_push(&state->readline_buff.buff, '\n');
		state->readline_buff.has_line = true;
	}
	return (return_new_line(state, ret));
}
