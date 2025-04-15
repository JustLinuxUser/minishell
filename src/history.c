#include "dsa/vec_str.h"
#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"
#include <fcntl.h>
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
		warning_error("Can't open the history file");
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
		warning_error("Can't open the history file");
	free(hist_file_path);
	free(hist.buff);
}

t_dyn_str	encode_cmd_hist(char *cmd)
{
	t_dyn_str	ret;

	dyn_str_init_alloc(&ret);
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

void	manage_history(t_state *state)
{
	char	*hist_entry;
	char	*enc_hist_entry;

	if (state->readline_buff.cursor > 1 && state->hist.hist_active
		&& (!state->hist.hist_cmds.len
			|| ft_strlen(
				state->hist.hist_cmds.buff[state->hist.hist_cmds.len - 1])
			!= state->readline_buff.cursor - 1
			|| ft_strncmp(
				state->hist.hist_cmds.buff[state->hist.hist_cmds.len - 1],
				state->readline_buff.buff.buff,
				state->readline_buff.cursor - 1)
		))
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
