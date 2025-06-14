/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   echo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:09 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 21:23:57 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	builtin_echo(t_state *state, t_vec_str argv)
{
	int		n;
	int		e;
	size_t	first_arg_print;

	n = 0;
	e = 0;
	(void)state;
	first_arg_print = parse_flags(argv, &n, &e);
	if (!print_args(e, argv, first_arg_print) && !n)
		ft_printf("\n");
	return (0);
}
