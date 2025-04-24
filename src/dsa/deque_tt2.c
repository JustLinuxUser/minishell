/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_tt2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 00:31:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/24 17:52:05 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"
#include "../libft/utils/utils.h"
#include "../libft/libft.h"

void	deque_tt_double_if_needed(t_deque_tt *ret)
{
	t_deque_tt	new;

	if (ret->cap > ret->len)
		return ;
	ft_assert(deque_tt_init(&new, ret->cap * 2 + 1) == 0);
	new.looking_for = ret->looking_for;
	while (ret->len)
	{
		deque_tt_push_end(&new, deque_tt_pop_start(ret));
	}
	free(ret->buff);
	ft_memcpy(ret, &new, sizeof(t_deque_tt));
}

t_token	*deque_tt_idx(t_deque_tt *ret, int idx)
{
	t_token	*i;

	ft_assert(idx < ret->len);
	i = &ret->buff[(ret->start + idx) % ret->cap];
	return (i);
}

int	deque_tt_clone(t_deque_tt *ret, const t_deque_tt proto)
{
	int	i;

	if (deque_tt_init(ret, proto.cap))
		return (1);
	i = 0;
	while (i < proto.len)
	{
		deque_tt_push_end(ret, proto.buff[(proto.start + i) % proto.cap]);
		i++;
	}
	return (0);
}

t_token	deque_tt_peek(t_deque_tt *ret)
{
	return (*deque_tt_idx(ret, 0));
}

void	deque_tt_clear(t_deque_tt *ret)
{
	ret->cap = 0;
	ret->start = 0;
	ret->len = 0;
}
