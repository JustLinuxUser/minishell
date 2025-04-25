#include "builtins.h"
#include <stdlib.h>

static void update_pwd_vars(t_state *state)
{
	t_env* PWD = env_get(&state->env,"PWD");
	
	if(PWD == NULL)
		try_unset(state, "OLDPWD");
	else
	{

	env_set(&state->env, (t_env){.exported = PWD->exported, .key = ft_strdup("OLDPWD"), 
										.value = ft_strdup(PWD->value)});
	}
	env_set(&state->env, (t_env){.exported = true, .key = ft_strdup("PWD"), 
										.value = ft_strdup(state->cwd.buff)});
}

int	builtin_cd(t_state *state, t_vec_str argv)
{
	char	*cwd;
	char	*home;
	int		e;
	char	*oldpwd;

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
	oldpwd = env_expand(state, "OLDPWD");
	if (!ft_strcmp("-", argv.buff[1]))
	{
		if (oldpwd == NULL)
		 	{
			ft_eprintf("%s: cd: OLDPWD not set\n", state->context);
			return(1);
			}
		ft_printf("%s\n",oldpwd);
		e = chdir(oldpwd);
	}
	else if (argv.len == 2) 
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
	update_pwd_vars (state);
	return (0);
}
//exit no imprime ext si no estoy en readline