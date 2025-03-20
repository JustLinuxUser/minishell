#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"

int	builtin_echo(t_state *state, t_vec_str argv)
{
	for (size_t i = 1; i < argv.len; i++)
	{
		ft_printf("%s ", vec_str_idx(&argv, i));
	}
	ft_printf("\n");
	return (0);
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
	return (0);
}

/*intento de CD*/
int	ft_cd(char **args)
{
	char	*target;
	char	old_pwd[1024];
	char	new_pwd[1024];

	// Save the current directory.
	if (getcwd(old_pwd, sizeof(old_pwd)) == NULL)
	{
		perror("minishell cd: getcwd");
		return (1);
	}
	// Determine the target directory.
	if (!args[1])
	{
		target = getenv("HOME");
		if (!target)
		{
			fprintf(stderr, "minishell: cd: HOME not set\n");
			return (1);
		}
	}
	else
	{
		target = args[1];
	}
	// Attempt to change the directory.
	if (chdir(target) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	// Update OLDPWD environment variable.
	if (setenv("OLDPWD", old_pwd, 1) != 0)
	{
		perror("minishell: cd: setenv OLDPWD");
		return (1);
	}
	// Get the new current directory.
	if (getcwd(new_pwd, sizeof(new_pwd)) == NULL)
	{
		perror("minishell cd: getcwd after chdir");
		return (1);
	}
	// Update PWD environment
	if (setenv("PWD", new_pwd, 1) != 0)
	{
		perror("minishell: cd: setenv PWD");
		return (1);
	}
	return (0);
}
