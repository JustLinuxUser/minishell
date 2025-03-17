/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_phandle_bonus.h                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2024/10/24 16:52:11 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_PHANDLE_BONUS_H
# define VEC_PHANDLE_BONUS_H
# include <stdlib.h>

typedef struct s_phandle
{
	int		pid;
	char	**args;
	char	*exe_path;
}	t_phandle;

typedef struct s_vec_phandle
{
	size_t		cap;
	size_t		len;
	t_phandle	*buff;
}	t_vec_phandle;
int			vec_phandle_init(t_vec_phandle *ret);
int			vec_phandle_double(t_vec_phandle *v);
int			vec_phandle_push(t_vec_phandle *v, t_phandle el);
t_phandle	vec_phandle_pop(t_vec_phandle *v);
t_phandle	vec_phandle_idx(t_vec_phandle *v, size_t idx);
#endif
