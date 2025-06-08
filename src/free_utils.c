/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/17 23:17:24 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/30 22:26:14 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <unistd.h>

void	free_redirects(t_vec_redir *v)
{
	size_t	i;
	t_redir	c;

	i = 0;
	while (i < v->len)
	{
		c = v->buff[i];
		if (c.should_delete)
			unlink(c.fname);
		close(c.fd);
		free(c.fname);
		i++;
	}
	free(v->buff);
	vec_redir_init(v);
}

void	free_all_state(t_state *state)
{
	free(state->input.buff);
	state->input = (t_dyn_str){};
	free(state->last_cmd_status_s);
	free(state->pid);
	free(state->context);
	free(state->base_context);
	state->context = 0;
	state->base_context = 0;
	free(state->readline_buff.buff.buff);
	free_redirects(&state->redirects);
	free_ast(&state->tree);
	free_hist(state);
	free(state->cwd.buff);
}

void	free_executable_cmd(t_executable_cmd cmd)
{
	size_t	i;

	i = 0;
	while (i < cmd.pre_assigns.len)
	{
		free(cmd.pre_assigns.buff[i].value);
		free(cmd.pre_assigns.buff[i].key);
		i++;
	}
	i = 0;
	while (i < cmd.argv.len)
	{
		free(cmd.argv.buff[i]);
		i++;
	}
	free(cmd.pre_assigns.buff);
	free(cmd.argv.buff);
}

void	free_executable_node(t_executable_node *node)
{
	free(node->redirs.buff);
	vec_int_init(&node->redirs);
}
