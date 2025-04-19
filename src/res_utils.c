/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   res_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 22:53:46 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 22:53:54 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_exe_res.h"
#include "libft/utils/utils.h"
#include <sys/wait.h>

t_exe_res	res_status(int status)
{
	return ((t_exe_res){.status = status, .pid = -1});
}

t_exe_res	res_pid(int pid)
{
	return ((t_exe_res){.status = -1, .pid = pid});
}

void	exe_res_set_status(t_exe_res *res)
{
	int	status;

	if (res->status != -1)
		return ;
	ft_assert(res->pid != -1);
	while (1)
		if (waitpid(res->pid, &status, 0) != -1)
			break ;
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		res->c_c = true;
	res->status = WEXITSTATUS(status)
		+ WIFSIGNALED(status) * 128 + WIFSIGNALED(status) * WTERMSIG(status);
}
