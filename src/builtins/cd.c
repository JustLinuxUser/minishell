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

static void	cd_check_args(t_state *state, t_vec_str argv, int *ret)
{
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n",
			state->context, argv.buff[0]);
		*ret = 1;
	}
}

static void	cd_do_chdir(t_state *state, t_vec_str argv,
				char *home, char *oldpwd, int *e, int *ret)
{
	if (argv.len == 1)
	{
		if (home == NULL)
		{
			ft_eprintf("%s: cd: HOME not set\n", state->context);
			*ret = 1;
			return ;
		}
		*e = chdir(home);
	}
	if (argv.len >= 2 && !ft_strcmp("-", argv.buff[1]))
	{
		if (oldpwd == NULL)
		{
			ft_eprintf("%s: cd: OLDPWD not set\n", state->context);
			*ret = 1;
			return ;
		}
		ft_printf("%s\n", oldpwd);
		*e = chdir(oldpwd);
	}
	else if (argv.len == 2)
	{
		*e = chdir(argv.buff[1]);
	}
}

static void	cd_refresh_cwd(t_state *state, t_vec_str argv, char *cwd)
{
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
		if (argv.len == 2)
			dyn_str_pushstr(&state->cwd, argv.buff[1]);
	}
}

int	builtin_cd(t_state *state, t_vec_str argv)
{
	char	*cwd;
	char	*home;
	int		e = 0;
	char	*oldpwd;
	int		ret = 0;

	cd_check_args(state, argv, &ret);
	if (ret)
		return (1);

	home = env_expand(state, "HOME");
	oldpwd = env_expand(state, "OLDPWD");
	cd_do_chdir(state, argv, home, oldpwd, &e, &ret);
	if (ret)
		return (1);

	if (e == -1)
	{
		ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0],
			argv.len >= 2 ? argv.buff[1] : "", strerror(errno));
		return (1);
	}

	cwd = getcwd(NULL, 0);
	cd_refresh_cwd(state, argv, cwd);
	free(cwd);
	update_pwd_vars(state);
	return (0);
}

//norminetear
