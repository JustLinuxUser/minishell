/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:00 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 21:21:32 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include <stdlib.h>

static void	update_pwd_vars(t_state *state)
{
	t_env	*pwd;

	pwd = env_get(&state->env, "PWD");
	if (pwd == NULL)
		try_unset(state, "OLDPWD");
	else
	{
		env_set(&state->env, (t_env){.exported = pwd->exported,
			.key = ft_strdup("OLDPWD"), .value = ft_strdup(pwd->value)});
	}
	env_set(&state->env, (t_env){.exported = true, .key = ft_strdup("PWD"),
		.value = ft_strdup(state->cwd.buff)});
}

static int	cd_check_args(t_state *state, t_vec_str argv)
{
	if (argv.len >= 3)
	{
		ft_eprintf("%s: %s: too many arguments\n", state->context,
			argv.buff[0]);
		return (1);
	}
	return (0);
}

static int	cd_do_chdir(t_state *state, t_vec_str argv, int *e)
{
	char	*oldpwd;

	oldpwd = env_expand(state, "OLDPWD");
	if (argv.len == 1)
	{
		return (cd_home(e, state));
	}
	if (argv.len >= 2 && !ft_strcmp("-", argv.buff[1]))
	{
		if (oldpwd == NULL)
		{
			ft_eprintf("%s: cd: OLDPWD not set\n", state->context);
			return (1);
		}
		ft_printf("%s\n", oldpwd);
		*e = chdir(oldpwd);
	}
	else if (argv.len == 2)
	{
		*e = chdir(argv.buff[1]);
	}
	return (0);
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
			" cannot access parent directories:"
			" No such file or directory \n");
		if (!dyn_str_ends_with_str(&state->cwd, "/") && state->cwd.buff)
			dyn_str_pushstr(&state->cwd, "/");
		if (argv.len == 2)
			dyn_str_pushstr(&state->cwd, argv.buff[1]);
	}
}

int	builtin_cd(t_state *state, t_vec_str argv)
{
	char	*cwd;
	int		e;
	char	*arg;

	arg = "";
	e = 0;
	if (cd_check_args(state, argv))
		return (1);
	if (cd_do_chdir(state, argv, &e))
		return (1);
	if (argv.len >= 2)
		arg = argv.buff[1];
	if (e == -1)
	{
		ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0], arg,
			strerror(errno));
		return (1);
	}
	cwd = getcwd(NULL, 0);
	cd_refresh_cwd(state, argv, cwd);
	free(cwd);
	update_pwd_vars(state);
	return (0);
}

// norminetear
