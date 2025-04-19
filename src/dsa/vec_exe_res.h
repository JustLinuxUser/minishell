/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vec_exe_res.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/13 22:52:15 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 22:54:38 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VEC_EXE_RES_H
# define VEC_EXE_RES_H
# include <stdbool.h>
# include <stdlib.h>

typedef struct s_exe_res
{
	// -1 means pending
	int		status;
	// -1 means no PID
	int		pid;
	bool	c_c;
}	t_exe_res;

typedef struct s_vec_exe_res
{
	size_t		cap;
	size_t		len;
	t_exe_res	*buff;
}	t_vec_exe_res;
int			vec_exe_res_init(t_vec_exe_res *ret);
int			vec_exe_res_double(t_vec_exe_res *v);
int			vec_exe_res_push(t_vec_exe_res *v, t_exe_res el);
t_exe_res	vec_exe_res_pop(t_vec_exe_res *v);
t_exe_res	vec_exe_res_idx(t_vec_exe_res *v, size_t idx);
t_exe_res	res_status(int status);
t_exe_res	res_pid(int pid);
void		exe_res_set_status(t_exe_res *res);
#endif
