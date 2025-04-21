#include "builtins.h"

int	builtin_pwd(t_state *state, t_vec_str argv)
{
	(void)argv;
	ft_printf("%s\n", state->cwd.buff);
	return (0);
}
