/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 22:51:28 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 22:51:29 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "libft/utils/utils.h"
#include "minishell.h"

#include <readline/history.h>
#include <unistd.h>

bool	worthy_of_being_remembered(t_state *state)
{
	if (state->readline_buff.cursor > 1 && state->hist.hist_active
		&& (!state->hist.hist_cmds.len
			|| !str_slice_eq_str(state->readline_buff.buff.buff,
				state->readline_buff.cursor - 1,
				state->hist.hist_cmds.buff[state->hist.hist_cmds.len - 1])
		))
	{
		return (true);
	}
	return (false);
}

void	manage_history(t_state *state)
{
	char	*hist_entry;
	char	*enc_hist_entry;

	if (worthy_of_being_remembered(state))
	{
		hist_entry = ft_strndup(state->readline_buff.buff.buff,
				state->readline_buff.cursor - 1);
		add_history(hist_entry);
		vec_str_push(&state->hist.hist_cmds, hist_entry);
		if (state->hist.append_fd >= 0)
		{
			enc_hist_entry = encode_cmd_hist(hist_entry).buff;
			if (write_to_file(enc_hist_entry, state->hist.append_fd))
			{
				warning_error("Failed to write to the history file");
				close(state->hist.append_fd);
				state->hist.append_fd = -1;
			}
			free(enc_hist_entry);
		}
	}
	buff_readline_reset(&state->readline_buff);
}

void	init_history(t_state *state)
{
	state->hist = (t_history){.append_fd = -1, .hist_active = true};
	parse_history_file(state);
}

void	free_hist(t_state *state)
{
	size_t	i;

	i = 0;
	while (i < state->hist.hist_cmds.len)
	{
		free(state->hist.hist_cmds.buff[i]);
		i++;
	}
	free(state->hist.hist_cmds.buff);
	vec_str_init(&state->hist.hist_cmds);
}
