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
	int	n;
	int	e;
	int	i;
	int	j;

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
			else if (*str == '\\')
				write(1, "\\", 1);
			else if (*str == 'r')
				write(1, "\r", 1);
			else if (*str == 'e')
				write(1, "\033", 1);
			else
			{
				write(1, "\\", 1);
				write(1, str, 1);
			}
			str++;
		}
		else
		{
			write(1, str, 1);
			str++;
		}
	}
}

static int	parse_flags(t_vec_str argv, int *n, int *e)
{
	int	i;
	int	j;

	i = 1;
	while (i < argv.len)
	{
		if (argv.buff[i][0] == '-' && argv.buff[i][1])
		{
			j = 1;
			while (argv.buff[i][j] == 'n' || argv.buff[i][j] == 'e')
				j++;
			if (argv.buff[i][j] != '\0')
				break ;
			j = 1;
			while (argv.buff[i][j])
			{
				if (argv.buff[i][j] == 'n')
					*n = 1;
				if (argv.buff[i][j] == 'e')
					*e = 1;
				j++;
			}
			i++;
		}
		else
			break ;
	}
	return (i);
}

static void	print_args(int e, t_vec_str argv, int i)
{
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
}

int	builtin_echo(t_state *state, t_vec_str argv)
{
	int	n;
	int	e;
	int	i;

	n = 0;
	e = 0;
	i = parse_flags(argv, &n, &e);
	print_args(e, argv, i);
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
		exit(0);
	err = ft_checked_atoi(argv.buff[1], &ret, 35);
	if (err != 0)
	{
		ft_eprintf("%s: %s: %s: numeric argument required\n", state->argv[0],
			argv.buff[0], argv.buff[1]);
		exit(2);
	}
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->argv[0],
			argv.buff[0]);
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
	cenv.key = ft_strdup("PWD");
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
	return (0);
}
