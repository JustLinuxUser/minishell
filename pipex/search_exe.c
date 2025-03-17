/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   search_exe.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 00:33:57 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 04:00:00 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "../libft/libft.h"
#include "../libft/dsa/dyn_str.h"
#include "pipex.h"
#include <unistd.h>

char	**find_path_dirs(char *envp[])
{
	int		i;
	char	*pathvar;

	i = 0;
	pathvar = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			pathvar = envp[i] + 5;
		i++;
	}
	if (!pathvar)
		return (0);
	return (ft_split(pathvar, ':'));
}

void	free_tab(char **tab)
{
	int	i;

	i = -1;
	while (tab[++i])
		free(tab[i]);
	free(tab);
}

char	*exe_path(t_config config, char *exe_name)
{
	char		**path_dirs;
	t_dyn_str	temp;
	int			i;

	if (ft_strchr(exe_name, '/') || ft_strlen(exe_name) == 0)
		return (ft_strdup(exe_name));
	path_dirs = find_path_dirs(config.envp);
	if (path_dirs == 0 || dyn_str_init(&temp))
		error_errno(config.argv[0]);
	i = -1;
	while (path_dirs[++i])
	{
		dyn_str_clear(&temp);
		if (dyn_str_pushstr(&temp, path_dirs[i])
			|| dyn_str_push(&temp, '/')
			|| dyn_str_pushstr(&temp, exe_name))
			error_errno(config.argv[0]);
		if (access(temp.buff, F_OK) == 0)
			break ;
		else if (path_dirs[i + 1] == 0)
			return (free_tab(path_dirs), free(temp.buff), NULL);
	}
	if (temp.len > 0)
		return (free_tab(path_dirs), temp.buff);
	return (free_tab(path_dirs), free(temp.buff), NULL);
}
