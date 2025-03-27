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

int builtin_echo(t_state *state, t_vec_str argv)
{
	int n = 0;
	int e = 0;
	int i = 1;
	int j = 0;

	while (i < argv.len)
	{
		if (argv.buff[i][0] == '-')
		{
			j = 1; // Empezamos después del '-'
			while (argv.buff[i][j] == 'n' || argv.buff[i][j] == 'e')
			{
				j++;
			}
			if (argv.buff[i][j] != '\0')
				break;

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
			break;
	}

	while (i < argv.len)
	{
		ft_printf("%s", argv.buff[i]);
		if (i < argv.len - 1)
			ft_printf(" ");
		i++;
	}

	if (!n)
		ft_printf("\n");

	ft_printf(" %d%d", e, n); // Debug
	return (0);
}
int builtin_pwd(t_state *state, t_vec_str argv)
{
char	*cwd;
cwd = getcwd(NULL, 0);
	ft_printf("%s\n",cwd);
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
/* my echo
void	ft_echo(char **args)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if(args[i] && strcmp(args[i], "-n") == 0)
	{
		newline = 0;
		i++;
	}
	while (args[i])
	{
		write(1, args[i], strlen(args[i]));
		if (args[i + 1])
			write(1, "", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
}
*/
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
