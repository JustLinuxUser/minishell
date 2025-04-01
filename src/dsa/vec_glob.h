/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_glob.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/28 11:20:11 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_GLOB_H
# define VEC_GLOB_H
# include <stdlib.h>

typedef enum e_glob
{
	G_PATTERN,
	G_SLASH,
	G_ASTRISK,
}	t_glob_tt;

typedef struct s_glob
{
	t_glob_tt	ty;
	char		*start;
	int			len;
}	t_glob;

typedef struct s_vec_glob
{
	size_t		cap;
	size_t		len;
	t_glob		*buff;
}	t_vec_glob;
int		vec_glob_init(t_vec_glob *ret);
int		vec_glob_double(t_vec_glob *v);
int		vec_glob_push(t_vec_glob *v, t_glob el);
t_glob	vec_glob_pop(t_vec_glob *v);
t_glob	vec_glob_idx(t_vec_glob *v, size_t idx);
#endif
