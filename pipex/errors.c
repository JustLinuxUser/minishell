/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/22 00:41:42 by anddokhn          #+#    #+#             */
/*   Updated: 2024/11/03 19:10:58 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../libft/ft_printf/ft_printf.h"

void	error_errno(char *progname)
{
	if (progname)
		perror(progname);
	else
		perror("ERROR");
	exit(1);
}

void	error_custom(char *e)
{
	ft_eprintf("ERROR: %s\n", e);
	exit(1);
}

void	error_errno_subj(char *progname, char *subject)
{
	ft_eprintf("%s: %s: %s\n", progname, subject, strerror(errno));
	exit(1);
}

void	error_errno_subj_log(char *progname, char *subject)
{
	ft_eprintf("%s: %s: %s\n", progname, subject, strerror(errno));
}

void	error_errno_subj_exit_code(char *progname, char *subject, int code)
{
	ft_eprintf("%s: %s: %s\n", progname, subject, strerror(errno));
	exit(code);
}
