/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 18:06:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/03 16:09:09 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../dsa/vec_str.h"
#include "../libft/dsa/dyn_str.h"
#include "../libft/libft.h"
#include "../minishell.h"
#include "../dsa/vec_glob.h"
#include <dirent.h>
#include <stdio.h>

void	get_next_path(t_dyn_str *next_path, char *path, char *fname)
{
	dyn_str_init(next_path);
	dyn_str_pushstr(next_path, path);
	dyn_str_pushstr(next_path, fname);
}

char	*get_curr_path(char *path)
{
	if (*path)
		return (path);
	else
		return (".");
}

int	process_dir(t_dir_matcher matcher)
{
	struct dirent	*diren;
	t_dyn_str		next_path;
	size_t			res;

	diren = readdir(matcher.dir);
	if (!diren)
		return (0);
	if (!ft_strcmp(diren->d_name, ".") || !ft_strcmp(diren->d_name, ".."))
		return (1);
	res = matches_pattern(diren->d_name, matcher.glob, matcher.offset, true);
	dyn_str_init(&next_path);
	if (res)
	{
		get_next_path(&next_path, matcher.path, diren->d_name);
		if (matcher.glob.len > res)
			dyn_str_push(&next_path, '/');
		else
		{
			vec_str_push(matcher.args, ft_strdup(next_path.buff));
			return (free(next_path.buff), 1);
		}
		match_dir(matcher.args, matcher.glob, next_path.buff, res + 1);
		free(next_path.buff);
	}
	return (1);
}

void	match_dir(t_vec_str *args, t_vec_glob glob, char *path, size_t offset)
{
	DIR				*dir;
	t_dir_matcher	matcher;

	dir = opendir(get_curr_path(path));
	if (glob.len <= offset && dir)
	{
		vec_str_push(args, ft_strdup(path));
		closedir(dir);
		return ;
	}
	if (!dir)
		return ;
	matcher = (t_dir_matcher){.path = path, .dir = dir,
		.glob = glob, .offset = offset, .args = args};
	while (!g_should_unwind && process_dir(matcher))
		;
	closedir(dir);
}

t_vec_str	expand_word_glob(t_ast_node word)
{
	t_vec_str	args;
	t_vec_glob	glob;

	vec_str_init(&args);
	glob = word_to_glob(word);
	if (glob.len == 0)
	{
		vec_str_push(&args, ft_strdup(""));
		return (args);
	}
	if (glob.buff[0].ty == G_SLASH)
		match_dir(&args, glob, "/", 1);
	else
		match_dir(&args, glob, "", 0);
	if (args.len == 0)
		vec_str_push(&args, word_to_string(word).buff);
	free(glob.buff);
	if (!g_should_unwind)
		ft_quicksort(&args);
	if (g_should_unwind)
		free_vec_str(&args);
	return (args);
}
