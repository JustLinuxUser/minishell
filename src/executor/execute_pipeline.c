/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_pipeline.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:45:10 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/05 13:41:42 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include <unistd.h>
#include "../libft/libft.h"

void	set_up_redir_pipeline_child(bool is_last, t_executable_node *exe,
	t_executable_node *curr_exe, int (*pp)[2])
{
	if (!is_last)
	{
		if (pipe(*pp))
			critical_error_errno_context("pipe");
		curr_exe->outfd = (*pp)[1];
		curr_exe->next_infd = (*pp)[0];
	}
	else
	{
		curr_exe->next_infd = -1;
		curr_exe->outfd = dup(exe->outfd);
	}
}

void	execute_pipeline_children(t_state *state, t_executable_node *exe,
	t_vec_exe_res *results)
{
	size_t				i;
	t_executable_node	curr_exe;
	int					pp[2];

	i = 0;
	curr_exe = (t_executable_node){};
	curr_exe.infd = dup(exe->infd);
	curr_exe.modify_parent_context = exe->node->children.len == 1;
	while (i < exe->node->children.len)
	{
		curr_exe.node = vec_nd_idx(&exe->node->children, i);
		ft_assert(curr_exe.node->node_type == AST_COMMAND);
		set_up_redir_pipeline_child(i == exe->node->children.len - 1,
			exe, &curr_exe, &pp);
		vec_exe_res_push(results, execute_command(state, &curr_exe));
		close(curr_exe.outfd);
		close(curr_exe.infd);
		curr_exe.infd = pp[0];
		i++;
	}
}

// Always returns status
t_exe_res	execute_pipeline(t_state *state, t_executable_node *exe)
{
	size_t				i;
	t_vec_exe_res		results;
	t_exe_res			res;

	results = (t_vec_exe_res){};
	execute_pipeline_children(state, exe, &results);
	res = res_status(0);
	i = 0;
	while (i < results.len)
	{
		res = vec_exe_res_idx(&results, i);
		exe_res_set_status(&res);
		i++;
	}
	free(results.buff);
	return (res);
}
