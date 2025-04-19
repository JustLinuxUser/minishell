/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tree_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 14:53:04 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:26:54 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <assert.h>

void	ast_postorder_traversal(t_ast_node *node, void (*f)(t_ast_node *node))
{
	size_t	i;

	i = 0;
	while (i < node->children.len)
	{
		ast_postorder_traversal(vec_nd_idx(&node->children, i), f);
		i++;
	}
	f(node);
}
