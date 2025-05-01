/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:26 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 22:17:51 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "../dsa/vec_env.h"
# include "../libft/dsa/dyn_str.h"
# include "../libft/ft_printf/ft_printf.h"
# include "../libft/libft.h"
# include "../minishell.h"
# include <errno.h>
# include <stdbool.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

int		mini_atoi_base(char **str, int base, int len);
int		cd_home(int *e, t_state *state);
void	parse_numeric_escape(char **str);
int		e_parser(char *str);
int		parse_flags(t_vec_str argv, int *n, int *e);
int		print_args(int e, t_vec_str argv, size_t i);
void	try_unset(t_state *state, char *key);

int		builtin_echo(t_state *state, t_vec_str argv);
int		builtin_pwd(t_state *state, t_vec_str argv);
int		builtin_exit(t_state *state, t_vec_str argv);
int		builtin_cd(t_state *state, t_vec_str argv);
int		builtin_env(t_state *state, t_vec_str argv);
int		builtin_export(t_state *state, t_vec_str argv);
int		builtin_unset(t_state *state, t_vec_str argv);

int	(*builtin_func(char *name))(t_state *state, t_vec_str argv);

#endif