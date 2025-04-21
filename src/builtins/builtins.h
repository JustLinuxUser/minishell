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

/* ───────── helpers de echo (5 en total) ───────── */
int		mini_atoi_base(char **str, int base, int len);
void	parse_numeric_escape(char **str);
void	e_parser(char *str);
int		parse_flags(t_vec_str argv, int *n, int *e, int *E);
void	print_args(int e, int E, t_vec_str argv, size_t i);

/* ───────── prototipos de built‑ins ───────── */
int		builtin_echo(t_state *state, t_vec_str argv);
int		builtin_pwd(t_state *state, t_vec_str argv);
int		builtin_exit(t_state *state, t_vec_str argv);
int		builtin_cd(t_state *state, t_vec_str argv);
int		builtin_env(t_state *state, t_vec_str argv);
int		builtin_export(t_state *state, t_vec_str argv);
int		builtin_unset(t_state *state, t_vec_str argv);

int (*builtin_func(char *name))(t_state *state, t_vec_str argv);

#endif