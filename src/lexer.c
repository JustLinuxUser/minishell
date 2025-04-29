/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 14:56:57 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/27 17:44:22 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>

bool	is_space(char c)
{
	if (c == ' ' || c == '\t')
		return (true);
	return (false);
}

bool	is_special_char(char c)
{
	char	*specials;

	specials = ";$'\"<>|&()\n";
	if (ft_strchr(specials, c) || is_space(c))
		return (true);
	return (false);
}

int	advance_dquoted(char **str)
{
	char	prev;

	assert(**str == '\"');
	(*str)++;
	prev = 0;
	while (**str && (**str != '\"' || prev == '\\'))
	{
		prev = **str;
		(*str)++;
	}
	if (**str != '\"')
		return (1);
	(*str)++;
	return (0);
}

int	advance_squoted(char **str)
{
	assert(**str == '\'');
	(*str)++;
	while (**str && **str != '\'')
	{
		(*str)++;
	}
	if (**str != '\'')
		return (1);
	(*str)++;
	return (0);
}
