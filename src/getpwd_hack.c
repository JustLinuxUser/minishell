/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   getpwd_hack.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:33:56 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:33:59 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"
#include <fcntl.h>
#include <unistd.h>

char	*getpid_hack(void)
{
	int			fd;
	t_dyn_str	file;
	char		*ret;
	char		**temp;

	fd = open("/proc/self/stat", O_RDONLY);
	if (fd < 0)
	{
		warning_error("Cannot get PID.");
		return (0);
	}
	dyn_str_init(&file);
	dyn_str_append_fd(fd, &file);
	close(fd);
	temp = ft_split(file.buff, ' ');
	free(file.buff);
	ret = ft_strdup(temp[0]);
	free_tab(temp);
	return (ret);
}
