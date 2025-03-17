/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_str.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/16 04:33:33 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_STR_H
# define VEC_STR_H
# include <stdlib.h>

typedef struct s_vec_str
{
	size_t	cap;
	size_t	len;
	char 	**buff;
}	t_vec_str;
int	vec_str_init(t_vec_str *ret);
int	vec_str_double(t_vec_str *v);
int	vec_str_push(t_vec_str *v, char * el);
char *	vec_str_pop(t_vec_str *v);
char *	vec_str_idx(t_vec_str *v, size_t idx);
#endif
