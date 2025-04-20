/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_assignment.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 21:23:14 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/20 22:55:53 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/dsa/dyn_str.h"
#include "../libft/libft.h"
#include "../minishell.h"
#include <assert.h>
#include <stdio.h>

t_dyn_str	word_to_string(t_ast_node node)
{
	t_token		curr;
	t_dyn_str	s;
	size_t		i;

	dyn_str_init(&s);
	i = 0;
	while (i < node.children.len)
	{
		assert(node.children.buff[i].node_type == AST_TOKEN);
		curr = node.children.buff[i].token;
		if (curr.tt == TT_WORD || curr.tt == TT_SQWORD
			|| curr.tt == TT_DQWORD || curr.tt == TT_DQENVVAR
			|| curr.tt == TT_ENVVAR)
		{
			assert(curr.start || !curr.len);
			dyn_str_pushnstr(&s, curr.start, curr.len);
		}
		else
		{
			printf("got unexpected: %s\n", tt_to_str(curr.tt));
			assert("Unreachable" == 0);
		}
		i++;
	}
	return (s);
}

t_dyn_str	word_to_hrdoc_string(t_ast_node node)
{
	t_token		curr;
	t_dyn_str	s;
	size_t		i;

	dyn_str_init(&s);
	i = 0;
	while (i < node.children.len)
	{
		assert(node.children.buff[i].node_type == AST_TOKEN);
		curr = node.children.buff[i].token;
		if (curr.tt == TT_WORD || curr.tt == TT_SQWORD
			|| curr.tt == TT_DQWORD)
			dyn_str_pushnstr(&s, curr.start, curr.len);
		else if (curr.tt == TT_DQENVVAR || curr.tt == TT_ENVVAR)
		{
			dyn_str_push(&s, '$');
			dyn_str_pushnstr(&s, curr.start, curr.len);
		}
		else
			assert("Unreachable" == 0);
		i++;
	}
	return (s);
}

t_env	assignment_to_env(t_state *state, t_ast_node *node)
{
	t_vec_str	args;
	t_env		ret;

	ret = (t_env){};
	vec_str_init(&args);
	assert(node->children.len == 2);
	expand_word(state, &node->children.buff[1], &args, true);
	assert(node->children.buff[1].children.buff == 0);
	ret.key = ft_strndup(node->children.buff[0].token.start,
			node->children.buff[0].token.len);
	if (args.len)
	{
		assert(args.len == 1);
		ret.value = args.buff[0];
		if (!ret.value)
			ret.value = ft_strdup("");
	}
	free(args.buff);
	return (ret);
}

void	assignment_word_to_word(t_ast_node *node)
{
	t_ast_node	ret;
	t_ast_node	left;
	t_ast_node	right;

	ret = (t_ast_node){.node_type = AST_WORD};
	assert(node->node_type == AST_ASSIGNMENT_WORD);
	assert(node->children.len == 2);
	left = node->children.buff[0];
	right = node->children.buff[1];
	left.token.len++;
	vec_nd_push(&ret.children, left);
	vec_nd_push_vec(&ret.children, &right.children);
	free(right.children.buff);
	free(node->children.buff);
	*node = ret;
}
