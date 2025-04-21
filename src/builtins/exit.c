#include "builtins.h"

int	builtin_exit(t_state *state, t_vec_str argv)
{
	int	ret, err;

	ft_eprintf("exit\n");
	if (argv.len == 1)
	{	free_all_state(state); exit(0); }

	err = ft_checked_atoi(argv.buff[1], &ret, 35);
	if (err)
	{	ft_eprintf("%s: %s: %s: numeric argument required\n",
			state->context, argv.buff[0], argv.buff[1]);
		free_all_state(state); exit(2); }

	if (argv.len >= 3)
	{	ft_eprintf("%s: %s: too many arguments\n",
			state->context, argv.buff[0]);
		free_all_state(state); return (1); }

	exit(ret);
}
