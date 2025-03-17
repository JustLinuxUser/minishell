/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   launch_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:38:45 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 04:07:07 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/ft_printf/ft_printf.h"
#include "pipex.h"
#include "vec_phandle.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void	launch_cmd_bg(t_config cfg, int pp[2], int stdin_fd, t_phandle *handle)
{
	close(pp[0]);
	dup2(stdin_fd, 0);
	close(stdin_fd);
	dup2(pp[1], 1);
	close(pp[1]);
	if (!handle->exe_path)
		handle->exe_path = handle->args[0];
	execve(handle->exe_path, handle->args, cfg.envp);
	if (errno == ENOENT)
		ft_eprintf("%s: %s: command not found\n", cfg.argv[0], handle->args[0]);
	else
		error_errno_subj_exit_code(cfg.argv[0], handle->exe_path, 127);
	exit(127);
}

int	launch_cmd(t_config config, t_vec_phandle *handles, char *cmd, int stdin_fd)
{
	int			pp[2];
	t_phandle	handle;

	handle.exe_path = exe_path(config, handle.args[0]);
	pipe(pp);
	handle.pid = fork();
	if (handle.pid == -1)
		error_errno(config.argv[0]);
	if (!handle.pid)
		launch_cmd_bg(config, pp, stdin_fd, &handle);
	close(pp[1]);
	close(stdin_fd);
	if (vec_phandle_push(handles, handle))
		error_errno(config.argv[0]);
	return (pp[0]);
}
