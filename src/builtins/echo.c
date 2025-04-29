#include "builtins.h"

int	builtin_echo(t_state *state, t_vec_str argv)
{
	int n;
	int e;
	size_t first_arg_print;

	n = 0;
	e = 0;
	(void) state;
	first_arg_print = parse_flags(argv, &n, &e);
	if (!print_args(e, argv, first_arg_print) && !n) 
		ft_printf("\n");
	return (0);
}