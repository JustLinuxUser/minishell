#include "libft/dsa/dyn_str.h"
#include "minishell.h"
#include "libft/libft.h"
#include <assert.h>
#include <readline/readline.h>
#include <stdbool.h>

void buff_readline_init(t_buff_readline *ret)
{
	*ret = (t_buff_readline){};
}

int buff_readline(t_buff_readline *l, t_dyn_str *ret, char *prompt)
{
	char	*temp;
	int		len;
	if (!l->has_line)
	{
		if (l->no_readline)
			return (0);
		temp = readline(prompt);
		if (!temp)
			return (0);
		dyn_str_pushstr(&l->buff, temp);
		dyn_str_push(&l->buff, '\n');
		free(temp);
		l->has_line = true;
	}
	// at this point we should have a newline
	temp = ft_strchr(l->buff.buff + l->cursor, '\n');
	ft_assert(temp != 0);
	len = temp - (l->buff.buff + l->cursor);
	dyn_str_pushnstr(ret, l->buff.buff + l->cursor, len);
	l->cursor += len + 1;
	l->has_line = l->cursor != l->buff.len;
	return ((len != 0) + 1);
}
