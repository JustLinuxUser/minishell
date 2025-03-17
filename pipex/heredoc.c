/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/24 16:40:49 by anddokhn          #+#    #+#             */
/*   Updated: 2025/03/17 04:00:25 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft/gnl/get_next_line.h"
#include "../libft/libft.h"
#include "pipex.h"
#include <fcntl.h>
#include <unistd.h>

void	heredoc_lines(t_config config, int heredoc_tmp)
{
	char	*line;
	int		size;

	while (1)
	{
		ft_printf("> ");
		line = get_next_line(0);
		if (line == 0)
			break ;
		if (ft_strncmp(line, config.argv[2], ft_strlen(config.argv[2])) == 0
			&& ft_strlen(line) - 1 == ft_strlen(config.argv[2]))
		{
			free(line);
			break ;
		}
		size = write(heredoc_tmp, line, ft_strlen(line));
		free(line);
		if (size < 0)
			error_errno(config.argv[0]);
	}
}

int	heredoc_fd(t_config config)
{
	int		heredoc_tmp;

	heredoc_tmp = open("/tmp/heredoc_tmp", O_WRONLY | O_TRUNC | O_CREAT, 0777);
	if (heredoc_tmp < 0)
		error_errno(config.argv[0]);
	heredoc_lines(config, heredoc_tmp);
	close(heredoc_tmp);
	heredoc_tmp = open("/tmp/heredoc_tmp", O_RDONLY);
	if (heredoc_tmp < 0)
		error_errno(config.argv[0]);
	return (heredoc_tmp);
}
