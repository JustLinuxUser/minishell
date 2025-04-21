#include "builtins.h"

int	builtin_env(t_state *state, t_vec_str argv)
{
	size_t i = 0; (void)argv;
	while (i < state->env.len)
	{
		if (state->env.buff[i].exported)
			ft_printf("%s=%s\n",
					  state->env.buff[i].key,
					  state->env.buff[i].value);
		++i;
	}
	return (0);
}