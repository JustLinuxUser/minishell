/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 14:53:38 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/10 14:54:26 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <stdlib.h>
#include <unistd.h>

void	free_tab(char **tab)
{
	int	i;

	i = 0;
	while (tab[i])
		free(tab[i++]);
	free(tab);
}

// 0 on success
int	write_to_file(char *str, int fd)
{
	int	wrote_total;
	int	wrote;
	int	len;

	len = ft_strlen(str);
	wrote_total = 0;
	while (wrote_total != len)
	{
		wrote = write(fd, str + wrote_total, len - wrote_total);
		if (wrote < 0)
			return (1);
		wrote_total += wrote;
	}
	return (0);
}

// Read fd untill the end, and append the results to *ret
void	dyn_str_append_fd(int fd, t_dyn_str *ret)
{
	char	buff[1024];
	int		len;

	while (1)
	{
		len = read(fd, buff, sizeof(buff));
		if (len == 0)
			break ;
		if (len > 0)
			dyn_str_pushnstr(ret, buff, len);
		else
			critical_error_errno();
	}
}
