#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <errno.h>
#include <string.h>


void critical_error(char *error) {
	ft_eprintf("[ERROR] %s\n", error);
	exit(1);
}

void critical_error_errno() {
	ft_eprintf("[ERROR] %s\n", strerror(errno));
	exit(1);
}

void warning_error( char *error) {
	ft_eprintf("[WARNING] %s\n", error);
}

void warning_error_errno() {
	ft_eprintf("[WARNING] %s\n", strerror(errno));
}
