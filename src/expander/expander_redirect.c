/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_redirect.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 21:27:11 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 19:07:16 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/ft_printf/ft_printf.h"
#include "../libft/utils/utils.h"
#include "../minishell.h"
#include <assert.h>
#include <fcntl.h>

bool	create_redir(t_tt tt, char *fname, t_redir *ret)
{
	ft_assert(tt != TT_HEREDOC && "HEREDOCS are handled separately");
	ret->fname = fname;
	ret->direction_in = tt == TT_REDIRECT_LEFT;
	if (!ret->fname)
		return (false);
	if (tt == TT_REDIRECT_LEFT)
		ret->fd = open(ret->fname, O_RDONLY);
	else if (tt == TT_REDIRECT_RIGHT)
		ret->fd = open(ret->fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else if (tt == TT_APPEND)
		ret->fd = open(ret->fname, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (ret->fd < 0)
	{
		free(ret->fname);
		return (false);
	}
	ret->should_delete = false;
	return (true);
}

int	redirect_from_ast_redir(t_state *state, t_ast_node *curr, int *redir_idx)
{
	t_redir		new_redir;
	t_tt		tt;
	t_token_old	full_token;
	char		*fname;

	assert(curr->node_type == AST_REDIRECT);
	if (curr->has_redirect)
	{
		*redir_idx = curr->redir_idx;
		return (0);
	}
	tt = curr->children.buff[0].token.tt;
	full_token = get_old_token(curr->children.buff[1]);
	fname = expand_word_single(state, vec_nd_idx(&curr->children, 1));
	if (!create_redir(tt, fname, &new_redir))
	{
		ft_eprintf("%s: %.*s: ambigous redirect\n",
			state->context, full_token.len, full_token.start);
		return (-1);
	}
	curr->redir_idx = state->redirects.len;
	*redir_idx = state->redirects.len;
	curr->has_redirect = true;
	vec_redir_push(&state->redirects, new_redir);
	return (0);
}
