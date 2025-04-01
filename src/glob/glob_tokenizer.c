/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   glob_tokenizer.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 15:25:40 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/30 15:26:46 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/libft.h"
#include "../dsa/vec_glob.h"
#include "../minishell.h"
#include <assert.h>

void	tokenize_star_glob(t_vec_glob *ret, t_token t, int *i)
{
	while (*i < t.len && t.start[*i] == '*')
		(*i)++;
	vec_glob_push(ret, (t_glob){.ty = G_ASTRISK});
}

bool	star_expandable(t_tt tt)
{
	if (tt == TT_SQWORD || tt == TT_DQWORD)
		return (false);
	else if (tt == TT_WORD || tt == TT_ENVVAR)
		return (true);
	else
		ft_assert("Unreachable" == 0);
	return (true);
}

void	tokenize_pattern(t_vec_glob *ret, t_token t, int *i)
{
	bool	expandable;
	char	*start;

	expandable = star_expandable(t.tt);
	start = t.start + *i;
	while (*i < t.len && t.start[*i] != '/'
		&& (t.start[*i] != '*' || !expandable))
		(*i)++;
	vec_glob_push(ret, (t_glob){.ty = G_PATTERN, .start = start,
		.len = t.start + *i - start});
}

void	tokenize_word_glob(t_vec_glob *ret, t_token t)
{
	int	i;

	i = 0;
	while (i < t.len)
	{
		if (t.start[i] == '/')
		{
			vec_glob_push(ret, (t_glob){.ty = G_SLASH});
			i++;
		}
		else if (t.start[i] == '*' && star_expandable(t.tt))
			tokenize_star_glob(ret, t, &i);
		else
			tokenize_pattern(ret, t, &i);
	}
}

t_vec_glob	word_to_glob(t_ast_node word)
{
	size_t		i;
	t_ast_node	curr_node;
	t_token		curr_t;
	t_vec_glob	ret;

	i = 0;
	vec_glob_init(&ret);
	while (i < word.children.len)
	{
		curr_node = *vec_nd_idx(&word.children, i);
		assert(curr_node.node_type == AST_TOKEN);
		curr_t = curr_node.token;
		tokenize_word_glob(&ret, curr_t);
		i++;
	}
	return (ret);
}
