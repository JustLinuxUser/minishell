/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exe_path.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:54:06 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:54:07 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"
#include <unistd.h>

char	*exe_path(char **path_dirs, char *exe_name)
{
	t_dyn_str	temp;
	int			i;

	if (ft_strchr(exe_name, '/') || ft_strlen(exe_name) == 0)
		return (ft_strdup(exe_name));
	dyn_str_init(&temp);
	i = -1;
	while (path_dirs[++i])
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

char	*find_cmd_path(t_state *state, t_vec_str *args)
{
	char	*path;
	char	**path_dirs;
	char	*path_of_exe;

	path = env_expand(state, "PATH");
	if (!path)
	{
		err_2(state, "command not found", args->buff[0]);
		free_all_state(state);
		return (0);
	}
	path_dirs = ft_split(path, ':');
	path_of_exe = exe_path(path_dirs, args->buff[0]);
	free_tab(path_dirs);
	if (!path_of_exe)
	{
		err_2(state, "command not found", args->buff[0]);
		free_all_state(state);
		return (0);
	}
	return (path_of_exe);
}
