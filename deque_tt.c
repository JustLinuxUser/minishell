/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_tt.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 00:31:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/02/08 18:22:51 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "minishell.h"
#include "libft/utils/utils.h"

int	deque_tt_init(t_deque_tt *ret, int size)
{
	*ret = (t_deque_tt){};
	ret->buff = malloc(size * sizeof(t_token));
	if (ret->buff == 0)
		return (1);
	ret->cap = size;
	return (0);
}

void	deque_tt_push_end(t_deque_tt *ret, t_token item)
{
	deque_tt_double_if_needed(ret);
	if (ret->len)
		ret->end = (ret->end + 1) % ret->cap;
	ret->buff[ret->end] = item;
	ret->len++;
}

void	deque_tt_push_start(t_deque_tt *ret, t_token item)
{
	deque_tt_double_if_needed(ret);
	if (ret->len)
		ret->start = ft_smod(ret->start - 1, ret->cap);
	ret->buff[ret->start] = item;
	ret->len++;
}

t_token	deque_tt_pop_end(t_deque_tt *ret)
{
	t_token	i;

	ft_assert(ret->len > 0);
	i = ret->buff[ret->end];
	ret->len--;
	if (ret->len)
		ret->end = ft_smod(ret->end - 1, ret->cap);
	return (i);
}

t_token	deque_tt_pop_start(t_deque_tt *ret)
{
	t_token	i;

	ft_assert(ret->len > 0);
	i = ret->buff[ret->start];
	ret->len--;
	if (ret->len)
		ret->start = (ret->start + 1) % ret->cap;
	return (i);
}
