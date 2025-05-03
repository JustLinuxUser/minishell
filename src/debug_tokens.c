/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tokens.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 17:45:12 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/03 16:11:02 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <stdio.h>
#include "libft/libft.h"

char	*tt_to_str_p2(t_tt tt)
{
	if (tt == TT_END)
		return ("TT_END");
	if (tt == TT_WORD)
		return ("TT_WORD");
	if (tt == TT_REDIRECT_LEFT)
		return ("TT_REDIRECT_LEFT");
	if (tt == TT_REDIRECT_RIGHT)
		return ("TT_REDIRECT_RIGHT");
	if (tt == TT_APPEND)
		return ("TT_APPEND");
	if (tt == TT_PIPE)
		return ("TT_PIPE");
	if (tt == TT_BRACE_LEFT)
		return ("TT_BRACE_LEFT");
	if (tt == TT_BRACE_RIGHT)
		return ("TT_BRACE_RIGHT");
	if (tt == TT_OR)
		return ("TT_OR");
	if (tt == TT_AND)
		return ("TT_AND");
	ft_assert(false);
	return (0);
}

char	*tt_to_str(t_tt tt)
{
	if (tt == TT_SEMICOLON)
		return ("TT_SEMICOLON");
	if (tt == TT_HEREDOC)
		return ("TT_HEREDOC");
	if (tt == TT_NEWLINE)
		return ("TT_NEWLINE");
	if (tt == TT_SQWORD)
		return ("TT_QWORD");
	if (tt == TT_DQWORD)
		return ("TT_DQWORD");
	if (tt == TT_ENVVAR)
		return ("TT_ENVVAR");
	if (tt == TT_DQENVVAR)
		return ("TT_DQENVVAR");
	return (tt_to_str_p2(tt));
}

void	print_tokens(t_deque_tt tokens)
{
	t_token	curr;
	int		i;

	i = 0;
	printf("------- PRINTING TOKENS --------\n");
	while (i < tokens.len)
	{
		curr = *deque_tt_idx(&tokens, i);
		printf("%s: >%.*s<\n", tt_to_str(curr.tt), curr.len, curr.start);
		i++;
	}
	printf("------- DONE --------\n");
}
