#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include "libft/libft.h"
#include <assert.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>

int	g_should_unwind = 0;

void	buff_readline_init(t_buff_readline *ret)
{
	*ret = (t_buff_readline){};
}

size_t	get_timestamp_micro(void)
{
	struct timeval	tv;

	gettimeofday(&tv, 0);
	return (tv.tv_usec / 1000 + (tv.tv_sec * 1000));
}

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

int	get_more_input_readline(t_buff_readline *l, char *prompt)
{
	int	pp[2];
	int	status;
	int	pid;

	if (pipe(pp))
		critical_error_errno();
	pid = fork();
	if (pid == 0)
	{
		// die_on_sig();
		default_signal_handlers();
		close(pp[0]);
		bg_readline(pp[1], prompt);
	}
	else if (pid < 0)
	{
		critical_error_errno();
	}
	else
	{
		close(pp[1]);
		dyn_str_append_fd(pp[0], &l->buff);
		buff_readline_update(l);
		close(pp[0]);
		wait(&status);
		if (WIFSIGNALED(status))
		{
			ft_printf("\n");
			return (2);
		}
		return (WEXITSTATUS(status));
	}
	ft_assert("Unreachable" == 0);
	return (0);
}

void update_context(t_state *state)
{
	if (!state->readline_buff.should_update_context)
		return ;
	free(state->context);
	state->context = ft_asprintf("%s: line %i", state->base_context, state->readline_buff.line);
}

int get_more_input_notty(t_state *state)
{
	char	buff[4096];
	int		ret;
	bool	has_read;

	has_read = false;
	while (1)
	{
		ret = read(0, buff, sizeof(buff));
		if (ret < 0)
			return (1);
		if (ret == 0)
			break ;
		has_read = true;
		dyn_str_pushnstr(&state->readline_buff.buff, buff, ret);
		if (ft_strnchr(buff, '\n', ret))
	  		break ;
	}
	buff_readline_update(&state->readline_buff);
	return (!has_read);
}

// 1 on ctrl-d
int	buff_readline(t_state *state, t_dyn_str *ret, char *prompt)
{
	int		code;
	char	*temp;
	int		len;

	if (!state->readline_buff.has_line)
	{
		if (state->input_method == INP_ARG
			|| state->input_method == INP_FILE)
			return (0);
		if (state->input_method == INP_STDIN_NOTTY)
			code = get_more_input_notty(state);
		else
			code = get_more_input_readline(&state->readline_buff, prompt);
		if (code == 1) // ctrl - d
			return (0);
		if (code == 2) // ctrl-c
		{
			g_should_unwind = 1;
			return (2);
		}
		dyn_str_push(&state->readline_buff.buff, '\n');
		state->readline_buff.has_line = true;
	}
	// at this point we should have a newline
	state->readline_buff.line++;
	update_context(state);
	temp = ft_strchr(state->readline_buff.buff.buff + state->readline_buff.cursor, '\n');
	ft_assert(temp != 0);
	len = temp - (state->readline_buff.buff.buff + state->readline_buff.cursor);
	dyn_str_pushnstr(ret, state->readline_buff.buff.buff + state->readline_buff.cursor, len);
	dyn_str_init_alloc(ret);
	state->readline_buff.cursor += len + 1;
	state->readline_buff.has_line = state->readline_buff.cursor != state->readline_buff.buff.len;
	if (len == 0)
		return (1);
	else
		return (4);
}

void	buff_readline_update(t_buff_readline *l)
{
	l->has_line = l->cursor != l->buff.len;
}

void	buff_readline_reset(t_buff_readline *l)
{
	ft_memmove(l->buff.buff, l->buff.buff + l->cursor, l->buff.len - l->cursor);
	l->buff.len -= l->cursor;
	l->cursor = 0;
}
