/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:40:58 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/19 07:41:16 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <errno.h>
#include <string.h>

void	err_cmd_not_found(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: command not found\n", state->context, cmd);
}

void	err_no_path_var(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: No such file or directory\n", state->context, cmd);
}

void	err_1_errno(t_state *state, char *p1)
{
	ft_eprintf("%s: %s: %s\n", state->context, p1, strerror(errno));
}

void	err_2(t_state *state, char *p1, char *p2)
{
	ft_eprintf("%s: %s: %s\n", state->context, p1, p2);
}
