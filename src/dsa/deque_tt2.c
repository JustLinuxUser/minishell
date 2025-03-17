/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deque_tt2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/15 00:31:41 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 18:35:56 by anddokhn         ###   ########.fr       */
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

t_token	*deque_tt_idx_wrapping(t_deque_tt *ret, int idx)
{
	t_token	*i;

	i = &ret->buff[ft_smod((ret->start + idx), ret->cap)];
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

t_token deque_tt_peek(t_deque_tt *ret)
{
	return (*deque_tt_idx(ret, 0));
}

t_token deque_tt_peek2(t_deque_tt *ret)
{
	return (*deque_tt_idx(ret, 1));
}

bool	deque_tt_check2(t_deque_tt *ret, t_tt t1, t_tt t2)
{
	if (ret->len < 2)
		return (false);
	if (deque_tt_idx(ret, 0)->tt == t1 && deque_tt_idx(ret, 1)->tt == t2)
		return (true);
	return (false);
}

bool	deque_tt_check1(t_deque_tt *ret, t_tt t1)
{
	if (ret->len < 1)
		return (false);
	if (deque_tt_idx(ret, 0)->tt == t1)
		return (true);
	return (false);
}
