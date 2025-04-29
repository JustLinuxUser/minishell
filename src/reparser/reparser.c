/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reparser.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 17:26:17 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 10:25:13 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

void	reparse_dquote(t_ast_node *ret, int *i, t_token t)
{
	int		prev_start;
	char	prev;

	assert(t.start[(*i)++] == '"');
	prev_start = *i;
	prev = 0;
	while (*i < t.len && (t.start[*i] != '"' || prev == '\\'))
	{
		if (t.start[*i] == '$')
		{
			vec_nd_push(&ret->children,
				create_subtoken_node(t, prev_start, *i, TT_DQWORD));
			reparse_envvar(ret, i, t, TT_DQENVVAR);
			prev_start = *i;
			continue ;
		}
		else if (t.start[*i] == '"')
			vec_nd_push(&ret->children,
				create_subtoken_node(t, prev_start, *i, TT_DQWORD));
		prev = t.start[*i];
		(*i)++;
	}
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, TT_DQWORD));
	assert(t.start[(*i)++] == '"');
}

void	reparse_squote(t_ast_node *ret, int *i, t_token t)
{
	int	prev_start;

	assert(t.start[(*i)++] == '\'');
	prev_start = *i;
	while (*i < t.len && t.start[*i] != '\'')
		(*i)++;
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, TT_SQWORD));
	assert(t.start[(*i)++] == '\'');
}

void	reparse_bs(t_ast_node *ret, int *i, t_token t)
{
	int	prev_start;

	assert(t.start[(*i)++] == '\\');
	prev_start = *i;
	if (*i == t.len)
		prev_start--;
	else
		(*i)++;
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, TT_WORD));
}

void	reparse_norm_word(t_ast_node *ret, int *i, t_token t)
{
	int	prev_start;

	prev_start = *i;
	while (*i < t.len && !is_special_char(t.start[*i]))
		(*i)++;
	vec_nd_push(&ret->children,
		create_subtoken_node(t, prev_start, *i, TT_WORD));
}

t_ast_node	reparse_word(t_token t)
{
	t_ast_node	ret;
	int			i;

	ret = (t_ast_node){.node_type = AST_WORD};
	i = 0;
	while (i < t.len)
	{
		if (t.start[i] == '"')
			reparse_dquote(&ret, &i, t);
		else if (t.start[i] == '\\')
			reparse_bs(&ret, &i, t);
		else if (t.start[i] == '\'')
			reparse_squote(&ret, &i, t);
		else if (t.start[i] == '$')
			reparse_envvar(&ret, &i, t, TT_ENVVAR);
		else if (is_space(t.start[i]))
			assert("Unreachable" == 0);
		else
			reparse_norm_word(&ret, &i, t);
	}
	return (ret);
}
