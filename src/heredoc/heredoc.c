/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 13:59:13 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/05 13:42:24 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/dsa/dyn_str.h"
#include "../libft/libft.h"
#include "../minishell.h"
#include <fcntl.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libft/libft.h"

// returns writable fd
int	ft_mktemp(t_state *state, t_ast_node *node)
{
	t_redir		ret;
	char		*temp;
	int			wr_fd;
	t_dyn_str	fname;

	ret = (t_redir){.direction_in = true, .should_delete = true};
	dyn_str_init(&fname);
	dyn_str_pushstr(&fname, "/tmp/heredoc_");
	if (state->pid)
		dyn_str_pushstr(&fname, state->pid);
	dyn_str_pushstr(&fname, "_");
	temp = ft_itoa(state->heredoc_idx++);
	dyn_str_pushstr(&fname, temp);
	ret.fname = fname.buff;
	free(temp);
	wr_fd = open(ret.fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (wr_fd < 0)
		critical_error_errno_context(ret.fname);
	ret.fd = open(ret.fname, O_RDONLY);
	if (ret.fd < 0)
		critical_error_errno_context(ret.fname);
	vec_redir_push(&state->redirects, ret);
	node->redir_idx = state->redirects.len - 1;
	node->has_redirect = true;
	return (wr_fd);
}

char	*first_non_tab(char *line)
{
	while (*line == '\t')
		line++;
	return (line);
}

int	env_len(char *line)
{
	int	len;

	len = 0;
	if (is_var_name_p1(line[len]))
		len++;
	else
		return (len);
	while (is_var_name_p2(line[len]))
		len++;
	return (len);
}

void	gather_heredoc(t_state *state, t_ast_node *node)
{
	int				wr_fd;
	t_dyn_str		sep;
	t_heredoc_req	req;

	ft_assert(node->children.len >= 1);
	if (node->children.buff[0].token.tt == TT_HEREDOC)
	{
		wr_fd = ft_mktemp(state, node);
		sep = word_to_hrdoc_string(node->children.buff[1]);
		ft_assert(sep.buff != 0);
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
