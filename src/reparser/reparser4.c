/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reparser4.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:05:45 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/06 18:05:46 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <stdbool.h>
#include "../libft/libft.h"
#include <stddef.h>

void	reparse_dq_bs(t_ast_node *ret, int *i, t_token t)
{
	ft_assert(t.start[*i] == '\\');
	(*i)++;
	if (ft_strchr("\"$\\", t.start[*i]))
		vec_nd_push(&ret->children,
			create_subtoken_node(t, *i, *i + 1, TT_SQWORD));
	else
		vec_nd_push(&ret->children,
			create_subtoken_node(t, *i - 1, *i + 1, TT_SQWORD));
	(*i)++;
}

void	reparse_dquote(t_ast_node *ret, int *i, t_token t)
{
	int		prev_start;
	bool	special;

	ft_assert(t.start[(*i)++] == '"');
	prev_start = *i;
	while (*i < t.len && t.start[*i] != '"')
	{
		special = t.start[*i] == '\\' || t.start[*i] == '$';
		if (special)
			vec_nd_push(&ret->children,
				create_subtoken_node(t, prev_start, *i, TT_DQWORD));
		if (t.start[*i] == '\\')
			reparse_dq_bs(ret, i, t);
		else if (t.start[*i] == '$')
			reparse_envvar(ret, i, t, TT_DQENVVAR);
		if (special)
		{
			prev_start = *i;
			continue ;
		}
		(*i)++;
	}
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, TT_DQWORD));
	ft_assert(t.start[(*i)++] == '"');
}
