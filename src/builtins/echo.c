#include "builtins.h"

int	builtin_echo(t_state *state, t_vec_str argv)
{
	int n = 0, e = 0, E = 0;
	size_t idx = parse_flags(argv, &n, &e, &E);

	(void)state;
	print_args(e, E, argv, idx);
	if (!n) ft_printf("\n");
	return (0);
}
