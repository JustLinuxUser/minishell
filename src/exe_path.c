/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:54:06 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/25 18:00:17 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"
#include <stdbool.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

char	*exe_path(char **path_dirs, char *exe_name)
{
	t_dyn_str	temp;
	int			i;

	if (ft_strchr(exe_name, '/'))
		return (ft_strdup(exe_name));
	if (ft_strlen(exe_name) == 0)
		return (0);
	dyn_str_init(&temp);
	i = -1;
	while (path_dirs && path_dirs[++i])
	{
		dyn_str_clear(&temp);
		dyn_str_pushstr(&temp, path_dirs[i]);
		dyn_str_push(&temp, '/');
		dyn_str_pushstr(&temp, exe_name);
		if (access(temp.buff, F_OK) == 0)
			break ;
		else if (path_dirs[i + 1] == 0)
			return (free(temp.buff), NULL);
	}
	if (temp.len > 0)
		return (temp.buff);
	return (free(temp.buff), NULL);
}

bool	check_is_a_dir(char *path, bool *enoent)
{
	struct stat	info;

	*enoent = false;
	if (stat(path, &info) == -1)
	{
		if (errno == ENOENT)
		{
			*enoent = true;
			return (false);
		}
		critical_error_errno_context(path);
	}
	return (S_ISDIR(info.st_mode));
}

static int	cmd_not_found(t_state *state, char *cmd_name)
{
	err_2(state, cmd_name, "command not found");
	free_all_state(state);
	return (COMMAND_NOT_FOUND);
}

static int	no_such_file_or_dir(t_state *state,
				char *cmd_name, char *path_of_exe)
{
	errno = ENOENT;
	err_1_errno(state, cmd_name);
	free_all_state(state);
	free(path_of_exe);
	return (COMMAND_NOT_FOUND);
}

int	find_cmd_path(t_state *state, char *cmd_name, char **path_of_exe)
{
	char	*path;
	char	**path_dirs;
	bool	enoent;

	path = env_expand(state, "PATH");
	path_dirs = 0;
	if (path)
		path_dirs = ft_split(path, ':');
	*path_of_exe = exe_path(path_dirs, cmd_name);
	if (path_dirs)
		free_tab(path_dirs);
	if (!*path_of_exe)
		return (cmd_not_found(state, cmd_name));
	if (check_is_a_dir(*path_of_exe, &enoent))
	{
		errno = EISDIR;
		err_1_errno(state, cmd_name);
		free_all_state(state);
		free(*path_of_exe);
		return (EXE_PERM_DENIED);
	}
	else if (enoent)
		return (no_such_file_or_dir(state, cmd_name, *path_of_exe));
	return (0);
}
