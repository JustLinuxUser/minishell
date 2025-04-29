/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffered_readline_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:22:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 10:19:17 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <errno.h>
#include <unistd.h>

void	buff_readline_update(t_buff_readline *l)
{
	l->has_line = l->cursor != l->buff.len;
}

void	buff_readline_reset(t_buff_readline *l)
{
	ft_memmove(l->buff.buff, l->buff.buff + l->cursor, l->buff.len - l->cursor);
	l->buff.len -= l->cursor;
	if (l->buff.buff)
		l->buff.buff[l->buff.len] = 0;
	l->cursor = 0;
	buff_readline_update(l);
}

void	buff_readline_init(t_buff_readline *ret)
{
	*ret = (t_buff_readline){};
}

void	update_context(t_state *state)
{
	if (!state->readline_buff.should_update_context)
		return ;
	free(state->context);
	state->context = ft_asprintf("%s: line %i",
			state->base_context, state->readline_buff.line);
}

int	get_more_input_notty(t_state *state)
{
	char	buff[4096 * 2];
	int		ret;
	int		status;

	status = 1;
	set_unwind_sig_norestart();
	while (1)
	{
		ret = read(0, buff, sizeof(buff));
		if (ret < 0 && errno == EINTR)
			status = 2;
		if (ret < 0 && errno == EINTR)
			break ;
		if (ret < 0)
			break ;
		if (ret == 0)
			break ;
		status = 0;
		dyn_str_pushnstr(&state->readline_buff.buff, buff, ret);
		if (ft_strnchr(buff, '\n', ret))
			break ;
	}
	set_unwind_sig();
	buff_readline_update(&state->readline_buff);
	return (status);
}
