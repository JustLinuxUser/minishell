/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 07:39:44 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/05 13:40:06 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <errno.h>
#include <string.h>

void	critical_error(char *error)
{
	ft_eprintf("[ERROR] %s\n", error);
	exit(1);
}

void	critical_error_errno(void)
{
	ft_eprintf("[ERROR] %s\n", strerror(errno));
	exit(1);
}

void	critical_error_errno_context(char *context)
{
	ft_eprintf("[ERROR] %s: %s\n", context, strerror(errno));
	exit(1);
}

void	warning_error(char *error)
{
	ft_eprintf("[WARNING] %s\n", error);
}

void	warning_error_errno(void)
{
	ft_eprintf("[WARNING] %s\n", strerror(errno));
}
