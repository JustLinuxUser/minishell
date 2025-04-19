/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_simple_list.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:56:33 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:56:34 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "libft/utils/utils.h"
#include "minishell.h"
#include <assert.h>

bool	should_execute(t_exe_res prev_status, t_tt prev_op)
{
	if (prev_status.c_c)
		return (false);
	ft_assert(prev_status.status != -1);
	assert(prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE
		|| prev_op == TT_AND || prev_op == TT_OR);
	if (prev_op == TT_SEMICOLON || prev_op == TT_NEWLINE)
		return (true);
	if (prev_op == TT_AND && prev_status.status == 0)
		return (true);
	if (prev_op == TT_OR && prev_status.status != 0)
		return (true);
	return (false);
}

t_exe_res	execute_simple_list(t_state *state, t_executable_node *exe)
{
	t_exe_res			status;
	t_tt				op;
	t_executable_node	exe_curr;
	size_t				i;

	status = res_status(0);
	op = TT_SEMICOLON;
	i = 0;
	while (i < exe->node->children.len)
	{
		exe_curr = *exe;
		exe_curr.node = &exe->node->children.buff[i];
		if (should_execute(status, op))
		{
			status = execute_tree_node(state, &exe_curr);
			assert(status.status != -1);
		}
		i++;
		if (i < exe->node->children.len)
			op = exe->node->children.buff[i].token.tt;
		i++;
	}
	return (status);
}
