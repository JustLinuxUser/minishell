/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 00:19:48 by anddokhn          #+#    #+#             */
/*   Updated: 2025/02/22 00:31:27 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <assert.h>

int exe_pipeline(t_ast_node node) {
}

int exe_simple_list(t_ast_node node) {
	t_ast_node curr;
    int i;
	int curr_res;
	t_tt curr_op;
	curr_res = exe_pipeline(node.children.buff[0]);
    i = 1;

    while (i < (int)node.children.len) {
		curr = node.children.buff[i];
		assert(curr.node_type == AST_COMMAND_PIPELINE);
		curr_op = node.children.buff[i].token.tt;
		i += 2;
    }
    return (0);
}
