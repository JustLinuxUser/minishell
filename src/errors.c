#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <errno.h>
#include <string.h>


void critical_error(char *error) {
	ft_eprintf("[ERROR] %s\n", error);
	exit(1);
}

void critical_error_errno(void) {
	ft_eprintf("[ERROR] %s\n", strerror(errno));
	exit(1);
}

void warning_error(char *error) {
	ft_eprintf("[WARNING] %s\n", error);
}

void warning_error_errno() {
	ft_eprintf("[WARNING] %s\n", strerror(errno));
}

void	err_cmd_not_found(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: command not found\n", state->context, cmd);
}

void	err_no_path_var(t_state *state, char *cmd)
{
	ft_eprintf("%s: %s: No such file or directory\n", state->context, cmd);
}

void	err_1_errno(t_state *state, char *p1)
{
	ft_eprintf("%s: %s: %s\n", state->context, p1, strerror(errno));
}

void	err_2(t_state *state, char *p1, char *p2)
{
	ft_eprintf("%s: %s: %s\n", state->context, p1, p2);
}
