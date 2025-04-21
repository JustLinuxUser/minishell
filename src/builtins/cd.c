#include "builtins.h"

int	builtin_cd(t_state *state, t_vec_str argv)
{
	int	e;

	char *cwd, *home = env_expand(state, "HOME");
	e = 0;
	if (argv.len >= 3)
		return (ft_eprintf("%s: %s: too many arguments\n", state->context,
				argv.buff[0]), 1);
	if (argv.len == 1)
	{
		if (!home)
			return (ft_eprintf("%s: cd: HOME not set\n", state->context), 1);
		e = chdir(home);
	}
	else
		e = chdir(argv.buff[1]);
	if (e == -1)
		ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0],
			argv.buff[1], strerror(errno));
	cwd = getcwd(NULL, 0);
	if (cwd)
	{
		state->cwd.len = 0;
		dyn_str_pushstr(&state->cwd, cwd);
	}
	else if (state->cwd.len > 0)
	{
		if (!dyn_str_ends_with_str(&state->cwd, "/"))
			dyn_str_pushstr(&state->cwd, "/");
		dyn_str_pushstr(&state->cwd, argv.len == 1 ? home : argv.buff[1]);
	}
	else
		ft_eprintf("%s: %s: %s: %s\n", state->context, argv.buff[0],
			argv.buff[1], strerror(errno));
	free(home);
	free(cwd);
	return (0);
}
