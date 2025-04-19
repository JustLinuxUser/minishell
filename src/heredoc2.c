/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 00:24:14 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 06:50:22 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>

// should brake
void	process_line(t_state *state, t_heredoc_req *req)
{
	t_dyn_str	alloc_line;
	int			stat;
	char		*line;

	dyn_str_init(&alloc_line);
	stat = buff_readline(state, &alloc_line, "heredoc> ");
	if (stat == 0 || stat == 2)
		return ((void)(req->finished = true));
	if ((req->full_file.len == 0
			|| req->full_file.buff[req->full_file.len - 1] == '\n')
		&& ft_strcmp(alloc_line.buff, req->sep) == 0)
	{
		free(alloc_line.buff);
		return ((void)(req->finished = true));
	}
	dyn_str_push(&alloc_line, '\n');
	if (req->remove_tabs)
		line = first_non_tab(alloc_line.buff);
	else
		line = alloc_line.buff;
	if (req->expand)
		expand_line(state, &req->full_file, line);
	else
		dyn_str_pushstr(&req->full_file, line);
	free(alloc_line.buff);
}

void	write_heredoc(t_state *state, int wr_fd, t_heredoc_req *req)
{
	while (!req->finished)
	{
		process_line(state, req);
	}
	if (req->full_file.len)
		assert(write_to_file(req->full_file.buff, wr_fd) == 0);
	free(req->full_file.buff);
}

bool	contains_quotes(t_ast_node node)
{
	size_t	i;

	if (node.node_type == AST_TOKEN
		&& (node.token.tt == TT_DQENVVAR || node.token.tt == TT_DQWORD
			|| node.token.tt == TT_SQWORD))
		return (true);
	i = 0;
	while (i < node.children.len)
	{
		if (contains_quotes(node.children.buff[i]))
			return (true);
	}
	return (false);
}

void	gather_heredoc(t_state *state, t_ast_node *node)
{
	int				wr_fd;
	t_dyn_str		sep;
	t_heredoc_req	req;

	assert(node->children.len >= 1);
	if (node->children.buff[0].token.tt == TT_HEREDOC)
	{
		wr_fd = ft_mktemp(state, node);
		sep = word_to_hrdoc_string(node->children.buff[1]);
		assert(sep.buff);
		req = (t_heredoc_req){
			.sep = sep.buff,
			.expand = !contains_quotes(node->children.buff[1]),
			.remove_tabs
			= ft_strncmp(node->children.buff[0].token.start, "<<-", 3)
			== 0};
		write_heredoc(state, wr_fd, &req);
		free(sep.buff);
	}
}

int	gather_heredocs(t_state *state, t_ast_node *node)
{
	size_t	i;

	i = 0;
	while (i < node->children.len && !g_should_unwind)
	{
		gather_heredocs(state, &node->children.buff[i]);
		i++;
	}
	if (node->node_type == AST_REDIRECT)
	{
		gather_heredoc(state, node);
	}
	return (0);
}
