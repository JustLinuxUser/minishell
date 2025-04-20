/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffered_readline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:23:53 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 22:45:46 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include "libft/libft.h"
#include <readline/readline.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

int	g_should_unwind = 0;

/* exit codes:
*
* 0 - verything ok
*
* 1 - ctrl-d
*
* 2 - ctrl-c / ctrl-\
*/
void	bg_readline(int outfd, char *prompt)
{
	char	*ret;

	ret = readline(prompt);
	if (!ret)
	{
		close(outfd);
		exit (1);
	}
	write_to_file(ret, outfd);
	close(outfd);
	exit(0);
}

int	attach_input_readline(t_buff_readline *l, int pp[2], int pid)
{
	int	status;

	close(pp[1]);
	dyn_str_append_fd(pp[0], &l->buff);
	buff_readline_update(l);
	close(pp[0]);
	while (1)
		if (waitpid(pid, &status, 0) != -1)
			break ;
	if (WIFSIGNALED(status))
	{
		ft_printf("\n");
		return (2);
	}
	return (WEXITSTATUS(status));
}

int	get_more_input_readline(t_buff_readline *l, char *prompt)
{
	int	pp[2];
	int	pid;

	if (pipe(pp))
		critical_error_errno();
	pid = fork();
	if (pid == 0)
	{
		readline_bg_signals();
		close(pp[0]);
		bg_readline(pp[1], prompt);
	}
	else if (pid < 0)
		critical_error_errno();
	else
	{
		return (attach_input_readline(l, pp, pid));
	}
	ft_assert("Unreachable" == 0);
	return (0);
}

int	return_new_line(t_state *state, t_dyn_str *ret)
{
	char	*temp;
	int		len;

	state->readline_buff.line++;
	update_context(state);
	temp = ft_strchr(state->readline_buff.buff.buff
			+ state->readline_buff.cursor, '\n');
	ft_assert(temp != 0);
	len = temp - (state->readline_buff.buff.buff
			+ state->readline_buff.cursor);
	if (len)
	{
		dyn_str_pushnstr(ret, state->readline_buff.buff.buff
			+ state->readline_buff.cursor, len);
	}
	state->readline_buff.cursor += len + 1;
	state->readline_buff.has_line = state->readline_buff.cursor
		!= state->readline_buff.buff.len;
	if (len == 0)
		return (1);
	return (4);
}

// 1 on ctrl-d
int	buff_readline(t_state *state, t_dyn_str *ret, char *prompt)
{
	int		code;

	if (!state->readline_buff.has_line)
	{
		if (state->input_method == INP_ARG || state->input_method == INP_FILE)
			return (0);
		if (state->input_method == INP_STDIN_NOTTY)
			code = get_more_input_notty(state);
		else
			code = get_more_input_readline(&state->readline_buff, prompt);
		if (code == 1)
			return (0);
		if (code == 2)
		{
			g_should_unwind = 1;
			set_cmd_status(state, (t_exe_res){.status = CANCELED, .c_c = true});
			return (2);
		}
		dyn_str_push(&state->readline_buff.buff, '\n');
		state->readline_buff.has_line = true;
	}
	return (return_new_line(state, ret));
}
