/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 00:24:14 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 12:47:10 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../minishell.h"
#include <assert.h>

bool	get_line_heredoc(t_state *state,
		t_heredoc_req *req, t_dyn_str *alloc_line)
{
	int			stat;

	dyn_str_init(alloc_line);
	stat = buff_readline(state, alloc_line, "heredoc> ");
	state->readline_buff.has_finished = false;
	if (stat == 0)
		ft_eprintf("%s: warning: here-document at"
			" line %i delimited by end-of-file (wanted `%s')\n",
			state->context, state->readline_buff.line, req->sep);
	if (stat == 0 || stat == 2)
	{
		req->finished = true;
		return (true);
	}
	return (false);
}

static bool	is_sep(t_heredoc_req *req, t_dyn_str *alloc_line)
{
	size_t	sep_len;

	sep_len = ft_strlen(req->sep);
	if ((req->full_file.len == 0
			|| req->full_file.buff[req->full_file.len - 1] == '\n'))
	{
		if (ft_strcmp(alloc_line->buff, req->sep) == 0)
		{
			return (true);
		}
		else if (alloc_line->buff[alloc_line->len - 1] == '\n'
			&& sep_len + 1 == alloc_line->len
			&& ft_strncmp(alloc_line->buff, req->sep, sep_len) == 0)
		{
			return (true);
		}
	}
	return (false);
}

// should brake
void	process_line(t_state *state, t_heredoc_req *req)
{
	t_dyn_str	alloc_line;
	char		*line;

	if (get_line_heredoc(state, req, &alloc_line))
		return ;
	if (is_sep(req, &alloc_line))
	{
		free(alloc_line.buff);
		return ((void)(req->finished = true));
	}
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
		i++;
	}
	return (false);
}
