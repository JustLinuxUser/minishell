/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 21:39:29 by armgonza          #+#    #+#             */
/*   Updated: 2025/04/16 23:15:21 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "dsa/vec_env.h"
#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <complex.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int	mini_atoi_base(char *str, int base, int len)
{
	int		i;
	int		res;
	char	strb[16] = "0123456789ABCDEF";
	char	strc;
	char	*strn;

	i = 0;
	res = 0;
	while (str[i] && len > i)
	{
		strc = ft_toupper(str[i]);
		strn = ft_strchr(strb, strc);
		if (strn)
		{
			res = (res * base) + (strn - strb);
		}
		else
		{
			break ;
		}
		i++;
	}
	return (res);
}

static int	is_hex_digit(char c)
{
	return ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A'
			&& c <= 'F'));
}

static void	parse_numeric_escape(char **str)
{
	char	val[4] = {0};
	int		len;
	int		base;

	len = 0;
	if (**str == '0')
		base = 8;
	else if (**str == 'x')
		base = 16;
	else
		return ;
	(*str)++;
	while (base == 8 && len < 3 && **str >= '0' && **str <= '7')
	{
		val[len] = **str;
		len++;
		(*str)++;
	}
	while (base == 16 && len < 2 && is_hex_digit(**str))
	{
		val[len] = **str;
		len++;
		(*str)++;
	}
	(*str)--;
	write(1, &(char){mini_atoi_base(val, base, len)}, 1);
}

static void	e_parser(char *str)
{
	while (*str)
	{
		if (*str == '\\' && *(str + 1))
		{
			str++;
			if (*str == 'n')
				write(1, "\n", 1);
			else if (*str == 't')
				write(1, "\t", 1);
			else if (*str == 'a')
				write(1, "\a", 1);
			else if (*str == 'b')
				write(1, "\b", 1);
			else if (*str == 'f')
				write(1, "\f", 1);
			else if (*str == 'r')
				write(1, "\r", 1);
			else if (*str == 'v')
				write(1, "\v", 1);
			else if (*str == '\\')
				write(1, "\\", 1);
			else if (*str == 'e')
				write(1, "\033", 1);
			else if (*str == 'c')
				return ;
			else if (*str == '0' || *str == 'x')
				parse_numeric_escape(&str);
			else
			{
				write(1, "\\", 1);
				write(1, str, 1);
			}
			str++;
		}
		else
			write(1, str++, 1);
	}
}

static int	parse_flags(t_vec_str argv, int *n, int *e, int *E)
{
	size_t	i;
	size_t	j;

	i = 1, *E = 0;
	while (i < argv.len)
	{
		if (argv.buff[i][0] == '-' && argv.buff[i][1])
		{
			j = 1;
			while (argv.buff[i][j] == 'n' || argv.buff[i][j] == 'e'
				|| argv.buff[i][j] == 'E')
				j++;
			if (argv.buff[i][j] != '\0')
				break ;
			j = 1;
			while (argv.buff[i][j])
			{
				if (argv.buff[i][j] == 'n')
					*n = 1;
				if (argv.buff[i][j] == 'e')
					*e = 1, *E = 0;
				if (argv.buff[i][j] == 'E')
					*E = 1, *e = 0;
				j++;
			}
			i++;
		}
		else
			break ;
	}
	return (i);
}

static void	print_args(int e, int E, t_vec_str argv, size_t i)
{
	while (i < argv.len)
	{
		if (e && !E)
			e_parser(argv.buff[i]);
		else
			ft_printf("%s", argv.buff[i]);
		if (i < argv.len - 1)
			ft_printf(" ");
		i++;
	}
}

int	builtin_echo(t_state *state, t_vec_str argv)
{
	int	n;
	int	e;
	int	E;
	int	i;

	E = 0;
	n = 0;
	e = 0;
	i = parse_flags(argv, &n, &e, &E);
	(void)state;
	print_args(e, E, argv, i);
	if (!n)
		ft_printf("\n");
	return (0);
}

int	builtin_pwd(t_state *state, t_vec_str argv)
{
	(void)state;
	(void)argv;
	ft_printf("%s\n", state->cwd.buff);
	return (0);
}
int	builtin_exit(t_state *state, t_vec_str argv)
{
	int	ret;
	int	err;

	ft_eprintf("exit\n");
	if (argv.len == 1)
	{
		free_all_state(state);
		exit(0);
	}
	err = ft_checked_atoi(argv.buff[1], &ret, 35);
	if (err != 0)
	{
		ft_eprintf("%s: %s: %s: numeric argument required\n", state->context,
			argv.buff[0], argv.buff[1]);
		free_all_state(state);
		exit(2);
	}
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->context,
			argv.buff[0]);
		free_all_state(state);
		return (1);
	}
	exit(ret);
}

static void	parse_export_arg(char *str, char **ident, char **val)
{
	char	*chr;
	int		len;

	chr = ft_strchr(str, '=');
	len = chr - str;
	if (chr != NULL)
	{
		*ident = ft_strndup(str, len);
		*val = ft_strdup(str + (len + 1));
	}
	else
	{
		*ident = ft_strdup(str);
		*val = NULL;
	}
}

static bool	is_valid_ident(char *ident)
{
	int	i;

	i = 0;
	if (is_var_name_p1(ident[0]) == false)
	{
		return (false);
	}
	while (ident[i] && is_var_name_p2(ident[i]))
	{
		i++;
	}
	return (!ident[i]);
}
int	builtin_env(t_state *state, t_vec_str argv)
{
	size_t	i;

	i = 0;
	(void)argv;
	while (state->env.len > i)
	{
		if(state->env.buff[i].exported)
			ft_printf("%s=%s\n", state->env.buff[i].key, state->env.buff[i].value);
		i++;
	}
	return (0);
}
int	builtin_export(t_state *state, t_vec_str argv)
{
	size_t	i;
	char	*val;
	char	*ident;
	t_env	*env;
	int		status;

	status = 0;
	i = 1;
	if (argv.len == 1)
		builtin_env(state, argv);
	while (i < argv.len)
	{
		parse_export_arg(argv.buff[i], &ident, &val);
		if (is_valid_ident(ident))
		{
			// aqui va la exportacion aL ENV
			if (val == NULL)
			{
				env = env_get(&state->env, ident);
				if (env)
				{
					env->exported = true;
				}
				free(ident);
			}
			else
			{
				env_set(&state->env, (t_env){.exported = true, .key = ident,
					.value = val});
			}
		}
		else
		{
			ft_eprintf("%s: %s: `%s' not valid identifier\n", state->context,
				argv.buff[0], argv.buff[i]);
			free(ident);
			free(val);
			status = 1;
		}
		i++;
	}
	return (status);
}

int	builtin_cd(t_state *state, t_vec_str argv)
{
	char	*cwd;
	char	*home;
	int		e;

	// t_env	cenv;
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->context,
			argv.buff[0]);
		return (1);
	}
	home = env_expand(state, "HOME");
	if (argv.len == 1)
	{
		if (home == NULL)
		{
			ft_eprintf("%s: cd: HOME not set\n", state->context);
			return (1);
		}
		e = chdir(home);
	}
	if (argv.len == 2)
	{
		e = chdir(argv.buff[1]);
	}
	if (e == -1)
	{
		ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0],
			argv.buff[1], strerror(errno));
	}
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		state->cwd.len = 0;
		dyn_str_pushstr(&state->cwd, cwd);
	}
	else
	{
		if (state->cwd.len > 0)
		{
			if (!dyn_str_ends_with_str(&state->cwd, "/"))
				dyn_str_pushstr(&state->cwd, "/");
			dyn_str_pushstr(&state->cwd, argv.buff[1]);
		}
		else
		{
			ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0],
				argv.buff[1], strerror(errno));
		}
	}
	free(home);
	free(cwd);
	return (0);
}

void	try_unset(t_state *state, char *str)
{
	size_t	index;
	t_env	*xenv;

	xenv = env_get(&state->env, str);
	if (!xenv)
	{
		return ;
	}
	index = xenv - state->env.buff;
	vec_env_del(&state->env, index);
}

int	builtin_unset(t_state *state, t_vec_str argv)
{
	size_t	i;

	i = 1;
	while (i < argv.len)
	{
		try_unset(state, argv.buff[i]);
		i++;
	}
	return (0);
}
int (*builtin_func(char *name))(t_state *state, t_vec_str argv)
{
	if (ft_strcmp(name, "echo") == 0)
	{
		return (builtin_echo);
	}
	else if (ft_strcmp(name, "export") == 0)
	{
		return (builtin_export);
	}
	else if (ft_strcmp(name, "cd") == 0)
	{
		return (builtin_cd);
	}
	else if (ft_strcmp(name, "exit") == 0)
	{
		return (builtin_exit);
	}
	else if (ft_strcmp(name, "pwd") == 0)
	{
		return (builtin_pwd);
	}
	else if (ft_strcmp(name, "env") == 0)
	{
		return (builtin_env);
	}
	else if (ft_strcmp(name, "unset") == 0)
	{
		return (builtin_unset);
	}
	return (0);
}
