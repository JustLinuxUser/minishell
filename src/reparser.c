/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reparser.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/05 17:26:17 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/10 14:48:24 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

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

void	reparse_envvar(t_ast_node *ret, int *i, t_token t, t_tt tt) {
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
	assert(*i < t.len);
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

void	reparse_words(t_ast_node	*node)
{
	t_ast_node	temp;
	t_token_old	full_word;
	t_token		tok;
	size_t		i;

	if (node->node_type == AST_WORD)
	{
		assert(node->children.len == 1);
		tok = node->children.buff[0].token;
		full_word = (t_token_old){.start = tok.start,
			.len = tok.len, .present = true};
		temp = *node;
		*node = reparse_word(node->children.buff[0].token);
		i = 0;
		while (i < node->children.len)
			node->children.buff[i++].token.full_word = full_word;
		free_ast(&temp);
	}
	i = 0;
	while (i < node->children.len)
		reparse_words(&node->children.buff[i++]);
}

bool	is_valid_ident(char *s, int len)
{
	int			i;

	i = 0;
	if (len <= 0 || !is_var_name_p1(s[0]))
		return (false);
	while (i < len)
	{
		if (!is_var_name_p2(s[i++]))
			return (false);
	}
	return (true);
}

void	reparse_assignment_word(t_ast_node *word)
{
	t_ast_node	new_root;
	t_token		*first_token;
	char		*eq;

	new_root = (t_ast_node){.node_type = AST_ASSIGNMENT_WORD};
	if (word->children.buff[0].token.tt == TT_WORD)
	{
		first_token = &word->children.buff[0].token;
		eq = ft_strnchr(first_token->start, '=', first_token->len);
		if (eq)
		{
			if (!is_valid_ident(first_token->start, eq - first_token->start))
				return ;
			vec_nd_push(&new_root.children,
				create_subtoken_node(*first_token, 0,
					eq - first_token->start, TT_WORD));
			first_token->len = first_token->len - (eq - first_token->start) - 1;
			first_token->start = eq + 1;
			vec_nd_push(&new_root.children, *word);
			*word = new_root;
		}
	}
}

void	reparse_assignment_words(t_ast_node *node)
{
	size_t	i;

	if (node->node_type != AST_REDIRECT)
	{
		i = 0;
		while (i < node->children.len)
			reparse_assignment_words(&node->children.buff[i++]);
	}
	if (node->node_type == AST_WORD)
		reparse_assignment_word(node);
}
