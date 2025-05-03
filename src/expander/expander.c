/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/20 21:26:20 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/03 16:09:44 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../dsa/vec_str.h"
#include "../minishell.h"
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libft/libft.h"

char	*expand_word_single(t_state *state, t_ast_node *curr)
{
	t_vec_str	args;
	size_t		i;
	char		*temp;

	vec_str_init(&args);
	expand_word(state, curr, &args, false);
	if (args.len != 1)
	{
		i = 0;
		while (i < args.len)
		{
			free(args.buff[i]);
			i++;
		}
		free(args.buff);
		return (0);
	}
	temp = args.buff[0];
	free(args.buff);
	return (temp);
}

t_token_old	get_old_token(t_ast_node word)
{
	t_token_old	ret;

	ft_assert(word.node_type == AST_WORD);
	ft_assert(word.children.len);
	ret = word.children.buff[0].token.full_word;
	ft_assert(ret.present);
	return (ret);
}

void	expand_node_glob(t_ast_node *node, t_vec_str *args, bool keep_as_one)
{
	t_vec_str	glob_words;
	t_dyn_str	temp;
	size_t		j;

	glob_words = expand_word_glob(*node);
	if (g_should_unwind)
		return ;
	dyn_str_init(&temp);
	j = 0;
	while (j < glob_words.len)
	{
		if (!keep_as_one)
			vec_str_push(args, glob_words.buff[j]);
		else
		{
			dyn_str_pushstr(&temp, glob_words.buff[j]);
			free(glob_words.buff[j]);
		}
		if (j++ + 1 < glob_words.len && keep_as_one)
			dyn_str_push(&temp, ' ');
	}
	if (keep_as_one)
		vec_str_push(args, temp.buff);
	free(glob_words.buff);
	free_ast(node);
}

void	expand_word(t_state *state, t_ast_node *node,
	t_vec_str *args, bool keep_as_one)
{
	t_vec_nd	words;
	size_t		i;

	expand_tilde_word(state, node);
	expand_env_vars(state, node);
	vec_nd_init(&words);
	if (!keep_as_one)
		words = split_words(state, node);
	else
	{
		vec_nd_push(&words, *node);
		*node = (t_ast_node){};
	}
	i = 0;
	while (i < words.len)
	{
		expand_node_glob(&words.buff[i], args, keep_as_one);
		if (g_should_unwind)
			while (i < words.len)
				free_ast(&words.buff[i++]);
		if (g_should_unwind)
			break ;
		i++;
	}
	return (free(words.buff), free_ast(node));
}
