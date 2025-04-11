#include "libft/dsa/dyn_str.h"
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
		die_on_sig();
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
			ft_eprintf("\n");
		}
		if (WEXITSTATUS(status) == 2)
		{
			ft_eprintf("\n");
		}
		return (WEXITSTATUS(status));
	}
	ft_assert("Unreachable" == 0);
	return (0);
}

// 1 on ctrl-d
int	buff_readline(t_buff_readline *l, t_dyn_str *ret, char *prompt)
{
	int		code;
	char	*temp;
	int		len;

	if (!l->has_line)
	{
		if (l->no_readline)
			return (0);
		code = get_more_input_readline(l, prompt);
		if (code == 1) // ctrl - d
			return (0);
		if (code == 2) // ctrl-c
		{
			g_should_unwind = 1;
			return (2);
		}
		dyn_str_push(&l->buff, '\n');
		l->has_line = true;
	}
	// at this point we should have a newline
	temp = ft_strchr(l->buff.buff + l->cursor, '\n');
	ft_assert(temp != 0);
	len = temp - (l->buff.buff + l->cursor);
	dyn_str_pushnstr(ret, l->buff.buff + l->cursor, len);
	dyn_str_init_alloc(ret);
	l->cursor += len + 1;
	l->has_line = l->cursor != l->buff.len;
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
