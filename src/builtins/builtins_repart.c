/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_repart.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:29 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 20:05:51 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

int	(*builtin_func(char *name))(t_state *state, t_vec_str argv)
{
	if (!name)
		return (0);
	if (!ft_strcmp(name, "echo"))
		return (builtin_echo);
	if (!ft_strcmp(name, "export"))
		return (builtin_export);
	if (!ft_strcmp(name, "cd"))
		return (builtin_cd);
	if (!ft_strcmp(name, "exit"))
		return (builtin_exit);
	if (!ft_strcmp(name, "pwd"))
		return (builtin_pwd);
	if (!ft_strcmp(name, "env"))
		return (builtin_env);
	if (!ft_strcmp(name, "unset"))
		return (builtin_unset);
	return (0);
}
