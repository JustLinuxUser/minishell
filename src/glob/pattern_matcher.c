/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pattern_matcher.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:28:46 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/19 11:26:11 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdbool.h>
#include <stddef.h>
#include "../minishell.h"
#include "../libft/libft.h"

bool	finished_pattern(t_vec_glob patt, size_t offset)
{
	t_glob	curr;

	if (patt.len == offset + 1)
		return (true);
	curr = patt.buff[offset + 1];
	if (curr.ty == G_SLASH)
		return (true);
	return (false);
}

size_t	match_g_pattern(char *name, t_vec_glob patt, size_t offset, bool first)
{
	t_glob	curr;
	char	*orig_name;

	orig_name = name;
	while (offset < patt.len && patt.buff[offset].ty == G_PATTERN)
	{
		curr = patt.buff[offset];
		if (ft_strncmp(curr.start, name, curr.len) != 0)
			return (0);
		if (finished_pattern(patt, offset))
		{
			if (name[curr.len] == 0)
				return (offset + 1);
			return (0);
		}
		offset++;
		name += curr.len;
	}
	if (first && (!ft_strcmp(orig_name, "..") || !ft_strcmp(orig_name, "."))
		&& offset != patt.len && patt.buff[offset].ty != G_SLASH)
		return (0);
	return (matches_pattern(name, patt, offset, false));
}

size_t	match_g_astrisk(char *name, t_vec_glob patt, int offset, bool first)
{
	int	res;

	if (first && *name == '.')
		return (0);
	while (*name)
	{
		if (!finished_pattern(patt, offset))
		{
			res = matches_pattern(name, patt, offset + 1, false);
			if (res)
				return (res);
		}
		name++;
	}
	if (finished_pattern(patt, offset))
		return (offset + 1);
	return (0);
}

size_t	matches_pattern(char *name, t_vec_glob patt, size_t offset, bool first)
{
	t_glob	curr;

	curr = patt.buff[offset];
	if (curr.ty == G_PATTERN)
		return (match_g_pattern(name, patt, offset, first));
	else if (curr.ty == G_ASTRISK)
		return (match_g_astrisk(name, patt, offset, first));
	return (0);
}
