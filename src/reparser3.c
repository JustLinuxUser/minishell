/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reparser3.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:12:52 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:12:59 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>

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
