/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 21:39:29 by armgonza          #+#    #+#             */
/*   Updated: 2025/03/31 23:14:58 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

/*int	builtin_echo(t_state *state, t_vec_str argv)
{
	for (size_t i = 1; i < argv.len; i++)
	{
		ft_printf("%s ", vec_str_idx(&argv, i));
	}
	ft_printf("\n");
	return (0);
}*/
/*
void	e_parser(char *str)
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
			else if (*str == '\\')
				write(1, "\\", 1);
			else if (*str == 'r')
				write(1, "\r", 1);
			else if (*str == 'e')
				write(1, "\033", 1);
			else
			{
				write(1, "\\", 1);
				write(1,str,1);
			}
			str++;
		}
		else
			write(1,str,1);
			str++;
	}
}
int	builtin_echo(t_state *state, t_vec_str argv)
{
	int		n;
	int		e;
	int		i;
	int		j;
	int		res;
	int		i;
	char	base[16] = "0123456789ABCDEF";
	int		i;
	int		res;
	char	strb[16] = "0l23456789ABCDEF";
	char	strc;
	char	*strn;
	int		i;
	int		res;
	char	strb[16] = "0l23456789ABCDEF";
	char	strc;
	char	*strn;

	n = 0;
	e = 0;
	i = 1;
	while (i < argv.len)
	{
		if (argv.buff[i][0] == '-' && argv.buff[i][1])
		{
			j = 1; // Empezamos después del '-'
			while (argv.buff[i][j] == 'n' || argv.buff[i][j] == 'e')
			{
				j++;
			}
			if (argv.buff[i][j] != '\0')
				break ;
			j = 1;
			while (argv.buff[i][j])
			{
				if (argv.buff[i][j] == 'n')
					n = 1;
				if (argv.buff[i][j] == 'e')
					e = 1;
				j++;
			}
			i++;
		}
		else
			break ;
	}
	while (i < argv.len)
	{
		if (!e)
		{
			ft_printf("%s", argv.buff[i]);
			if (i < argv.len - 1)
				ft_printf(" ");
		}
		else
		{
			e_parser(argv.buff[i]);
			if (i < argv.len - 1)
				ft_printf(" ");
		}
		i++;
	}
if (!n)
ft_printf("\n");
return (0);
} */
mini_atoi_base(char *str, int base, int len)
{
	int i = 0;
	int res;
	char strb[16] = "0123456789ABCDEF";
	char strc;
	char *strn;

	while (str[i] && len > i)
	{
		strc=ft_toupper(str[i]);
		
		strn = ft_strchr(strb, strc);
		if (strn)
		{	
			res = (res * base) + (strn - strb);
		}
		else
		{
			break;
		}
		i++;
	}
	return (res);
}

static int	is_hex_digit(char c)
{
	return ((c >= '0' && c <= '9') ||
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F'));
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
	int	i;
	int j;
	i = 1,
	*E = 0;
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

static void print_args(int e, int E, t_vec_str argv, int i)
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
	print_args(e, E, argv, i);
	if (!n)
		ft_printf("\n");
	return (0);
}

int	builtin_pwd(t_state *state, t_vec_str argv)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	ft_printf("%s\n", cwd);
}
int	builtin_exit(t_state *state, t_vec_str argv)
{
	int	ret;
	int	err;

	ft_eprintf("exit\n");
	if (argv.len == 1)
	{
		free_all_state(*state);
		exit(0);
	}
	err = ft_checked_atoi(argv.buff[1], &ret, 35);
	if (err != 0)
	{
		ft_eprintf("%s: %s: %s: numeric argument required\n", state->argv[0],
			argv.buff[0], argv.buff[1]);
		free_all_state(*state);
		exit(2);
	}
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->argv[0],
			argv.buff[0]);
		free_all_state(*state);
		return (1);
	}
	exit(ret);
}

int	builtin_cd(t_state *state, t_vec_str argv)
{
	char	*cwd;
	char	*home;
	t_env	cenv;
	int		e;

	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->argv[0],
			argv.buff[0]);
		return (1);
	}
	cwd = getcwd(NULL, 0);
	cenv.key = ft_strdup("OLDPWD");
	cenv.value = cwd;
	cenv.exported = true;
	env_set(&state->env, cenv);
	home = env_expand(state, "HOME");
	if (argv.len == 1)
	{
		if (home == NULL)
		{
			ft_eprintf("%s: cd: HOME not set\n", state->argv[0]);
			return (1);
		}
		chdir(home);
	}
	if (argv.len == 2)
	{
		e = chdir(argv.buff[1]);
		if (e == -1)
			ft_eprintf("%s: %s: %s: %s\n", state->argv[0], argv.buff[0],
				argv.buff[1], strerror(errno));
	}
	cwd = getcwd(NULL, 0);
	cenv.key = ft_strdup(ft_strdup("PWD"));
	cenv.value = cwd;
	cenv.exported = true;
	env_set(&state->env, cenv);
}

int	builtin_export(t_state *state, t_vec_str argv)
{
	env_set(&state->env, (t_env){.key = ft_strdup("hello"),
		.value = ft_strdup("world")});
	return (0);
}

int (*builtin_func(char *name))(t_state *state, t_vec_str argv)
{
	if (ft_strcmp(name, "echo") == 0)
	{
		return (builtin_echo);
	}
	if (ft_strcmp(name, "export") == 0)
	{
		return (builtin_export);
	}
	if (ft_strcmp(name, "cd") == 0)
	{
		return (builtin_cd);
	}
	if (ft_strcmp(name, "exit") == 0)
	{
		return (builtin_exit);
	}
	if (ft_strcmp(name, "pwd") == 0)
	{
		return (builtin_pwd);
	}
	if (ft_strcmp(name, "export") == 0)
	{
		return (builtin_export);
	}
	return (0);
}