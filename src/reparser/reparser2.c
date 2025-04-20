/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reparser2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:09:10 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 22:57:32 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../minishell.h"
#include <assert.h>
#include <stdbool.h>

//[a-zA-Z_]
bool	is_var_name_p1(char c)
{
	if (ft_isalpha(c) || c == '_')
		return (true);
	return (false);
}

//[a-zA-Z0-9_]
bool	is_var_name_p2(char c)
{
	if (ft_isalnum(c) || c == '_')
		return (true);
	return (false);
}

bool	reparse_special_envvar(t_ast_node *ret, int *i, t_token t, t_tt tt)
{
	int		prev_start;
	char	c;

	prev_start = *i;
	c = t.start[*i];
	if (c != '?' && c != '$')
		return (false);
	(*i)++;
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, tt));
	return (true);
}

void	reparse_envvar(t_ast_node *ret, int *i, t_token t, t_tt tt)
{
	int	prev_start;

	assert(t.start[(*i)++] == '$');
	prev_start = *i;
	if (reparse_special_envvar(ret, i, t, tt))
		return ;
	if (*i < t.len && is_var_name_p1(t.start[*i]))
		(*i)++;
	while (*i < t.len && is_var_name_p2(t.start[*i]))
	{
		(*i)++;
	}
	if (prev_start == *i + 1)
	{
		vec_nd_push(&ret->children,
			create_subtoken_node(t, prev_start - 1, *i, TT_WORD));
	}
	else
	{
		vec_nd_push(&ret->children,
			create_subtoken_node(t, prev_start, *i, tt));
	}
}
