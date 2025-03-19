#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"

int builtin_echo(t_state *state, t_vec_str argv)
{
	for (size_t i = 1; i < argv.len; i++)
	{
		ft_printf("%s ", vec_str_idx(&argv, i));
	}
	ft_printf("\n");
	return 0;
}

int	builtin_export(t_state *state, t_vec_str argv)
{
	env_set(&state->env, (t_env){.key = ft_strdup("hello"), .value = ft_strdup("world")});
	return 0;
}

int (*builtin_func(char *name)) (t_state *state, t_vec_str argv) {

	if (ft_strcmp(name, "echo") == 0) {
		return (builtin_echo);
	}
	if (ft_strcmp(name, "export") == 0) {
		return (builtin_export);
	}
	return (0);
}
