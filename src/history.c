/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 22:51:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 20:15:39 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_str.h"
#include "libft/dsa/dyn_str.h"
#include "minishell.h"
#include <fcntl.h>
#include <stdio.h>

#include <readline/history.h>
#include <stdbool.h>
#include <unistd.h>

t_dyn_str	parse_single_cmd(t_dyn_str hist, size_t *cur)
{
	t_dyn_str	cmd;
	bool		bs;
	char		c;

	dyn_str_init(&cmd);
	bs = false;
	while (*cur < hist.len)
	{
		c = hist.buff[*cur];
		if (c == '\\' && !bs)
		{
			bs = true;
			(*cur)++;
			continue ;
		}
		if (c == '\n' && !bs)
		{
			(*cur)++;
			break ;
		}
		dyn_str_push(&cmd, c);
		bs = false;
		(*cur)++;
	}
	return (cmd);
}

t_vec_str	parse_hist_file(t_dyn_str hist)
{
	size_t		cur;
	t_vec_str	ret;
	char		*cmd;

	cur = 0;
	vec_str_init(&ret);
	while (cur < hist.len)
	{
		cmd = parse_single_cmd(hist, &cur).buff;
		vec_str_push(&ret, cmd);
		add_history(cmd);
	}
	return (ret);
}

char	*get_hist_file_path(t_state *state)
{
	t_env		*env;
	t_dyn_str	full_path;

	env = env_get(&state->env, "HOME");
	if (!env || !env->value)
	{
		warning_error("HOME is not set, can't get the history");
		return (0);
	}
	dyn_str_init(&full_path);
	dyn_str_pushstr(&full_path, env->value);
	if (!dyn_str_ends_with_str(&full_path, "/"))
		dyn_str_push(&full_path, '/');
	dyn_str_pushstr(&full_path, HIST_FILE);
	return (full_path.buff);
}

void	parse_history_file(t_state *state)
{
	t_dyn_str	hist;
	int			fd;
	char		*hist_file_path;

	hist_file_path = get_hist_file_path(state);
	if (!hist_file_path)
		return ;
	fd = open(hist_file_path, O_RDONLY | O_CREAT, 0666);
	if (fd < 0)
	{
		warning_error("Can't open the history file for reading");
		free(hist_file_path);
		return ;
	}
	dyn_str_init(&hist);
	dyn_str_append_fd(fd, &hist);
	close(fd);
	state->hist.hist_cmds = parse_hist_file(hist);
	state->hist.append_fd
		= open(hist_file_path, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (state->hist.append_fd < 0)
		warning_error("Can't open the history file for writing");
	free(hist_file_path);
	free(hist.buff);
}

t_dyn_str	encode_cmd_hist(char *cmd)
{
	t_dyn_str	ret;

	dyn_str_init(&ret);
	while (*cmd)
	{
		if (*cmd == '\\')
			dyn_str_push(&ret, '\\');
		if (*cmd == '\n')
			dyn_str_push(&ret, '\\');
		dyn_str_push(&ret, *cmd);
		cmd++;
	}
	dyn_str_push(&ret, '\n');
	return (ret);
}
