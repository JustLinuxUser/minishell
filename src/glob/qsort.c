/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   qsort.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 22:52:19 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 22:52:36 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../dsa/vec_str.h"
#include "../libft/libft.h"

void	swap_sort_point(char **p1, char **p2)
{
	char	*t;

	t = *p1;
	*p1 = *p2;
	*p2 = t;
}

int	partition(t_vec_str *vec, int low, int high)
{
	int		i;
	int		p_idx;
	int		j;

	p_idx = (high - low) / 2 + low;
	swap_sort_point(&vec->buff[low], &vec->buff[p_idx]);
	i = low;
	j = high;
	while (i < j)
	{
		while (ft_strcmp(vec->buff[i], vec->buff[low]) < 0 && i <= high - 1)
			i++;
		while (ft_strcmp(vec->buff[j], vec->buff[low]) >= 0 && j >= low + 1)
			j--;
		if (i < j)
			swap_sort_point(&vec->buff[i], &vec->buff[j]);
	}
	return (i);
}

void	quicksort_inner(t_vec_str *vec, int low, int high)
{
	int	p;

	if (low < high)
	{
		p = partition(vec, low, high);
		quicksort_inner(vec, low, p - 1);
		quicksort_inner(vec, p + 1, high);
	}
}

void	ft_quicksort(t_vec_str *vec)
{
	if (vec->len > 1)
		quicksort_inner(vec, 0, vec->len - 1);
}
