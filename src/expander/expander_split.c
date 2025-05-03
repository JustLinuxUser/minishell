/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_split.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 08:11:39 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/03 16:09:09 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../dsa/vec_env.h"
#include "../libft/libft.h"
#include "../minishell.h"

void	expand_token(t_state *state, t_token	*curr_tt)
{
	char	*temp;

	temp = env_expand_n(state, curr_tt->start, curr_tt->len);
	curr_tt->start = temp;
	if (curr_tt->start)
		curr_tt->len = ft_strlen(temp);
	else
		curr_tt->len = 0;
	curr_tt->allocated = false;
}

void	expand_env_vars(t_state *state, t_ast_node *node)
{
	t_token	*curr_tt;
	size_t	i;

	i = 0;
	while (i < node->children.len)
	{
		ft_assert(node->children.buff[i].node_type == AST_TOKEN);
		curr_tt = &node->children.buff[i].token;
		if (curr_tt->tt == TT_WORD || curr_tt->tt == TT_SQWORD
			|| curr_tt->tt == TT_DQWORD)
		{
		}
		else if (curr_tt->tt == TT_DQENVVAR || curr_tt->tt == TT_ENVVAR)
		{
			expand_token(state, curr_tt);
		}
		else
			ft_assert("Unreachable" == 0);
		i++;
	}
}

t_ast_node	new_env_node(char *new_start)
{
	return ((t_ast_node){.node_type = AST_TOKEN,
		.token = {.allocated = true,
			.len = ft_strlen(new_start),
			.start = new_start,
			.tt = TT_ENVVAR}});
}

void	split_envvar(t_state *state, t_token *curr_t,
			t_ast_node *curr_node, t_vec_nd *ret)
{
	char	**things;
	int		i;

	if (!curr_t->start)
		return ;
	things = ft_split_str(curr_t->start, env_get_ifs(&state->env));
	if (things[0])
	{
		vec_nd_push(&curr_node->children, new_env_node(things[0]));
		i = 1;
		while (things[i])
		{
			vec_nd_push(ret, *curr_node);
			*curr_node = (t_ast_node){.node_type = AST_WORD};
			vec_nd_push(&curr_node->children, new_env_node(things[i]));
			i++;
		}
	}
	else if (curr_node->children.len)
	{
		vec_nd_push(ret, *curr_node);
		*curr_node = (t_ast_node){.node_type = AST_WORD};
	}
	free(things);
}

// node -> split node
t_vec_nd	split_words(t_state *state, t_ast_node *node)
{
	t_vec_nd	ret;
	t_token		*curr_t;
	t_ast_node	curr_node;
	int			i;

	vec_nd_init(&ret);
	curr_node = (t_ast_node){.node_type = AST_WORD};
	i = -1;
	while (++i < (int)node->children.len)
	{
		ft_assert(node->children.buff[i].node_type == AST_TOKEN);
		curr_t = &node->children.buff[i].token;
		if (curr_t->tt == TT_WORD || curr_t->tt == TT_SQWORD
			|| curr_t->tt == TT_DQWORD || curr_t->tt == TT_DQENVVAR)
			vec_nd_push(&curr_node.children, node->children.buff[i]);
		else if (curr_t->tt == TT_ENVVAR)
			split_envvar(state, curr_t, &curr_node, &ret);
		else
			ft_assert("Unreachable" == 0);
	}
	if (curr_node.children.len)
		vec_nd_push(&ret, curr_node);
	free(node->children.buff);
	*node = (t_ast_node){};
	return (ret);
}
