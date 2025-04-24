#include "builtins.h"

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
			return(1);
	}
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		state->cwd.len = 0;
		dyn_str_pushstr(&state->cwd, cwd);
	}
	else
	{		
		ft_eprintf("cd: error retrieving current directory: getcwd:"
				" cannot access parent directories: No such file or directory \n");
		if (!dyn_str_ends_with_str(&state->cwd, "/") && state->cwd.buff)
			dyn_str_pushstr(&state->cwd, "/");
		dyn_str_pushstr(&state->cwd, argv.buff[1]);
	}
	free(cwd);
	return (0);
}
//to do  errors;

//manejar el entorno de pwd y oldpwd

// inicializacion de la minishell el getcwd al inicio de la minishell (errores en caso de que nos quiten el entorno etc bla)

// cd - // cd oldpwd fin

//norminettear todo