#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <readline/readline.h>
#include <signal.h>
#include <stdlib.h>

void	default_signal_handlers(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	readline_bg_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
}

void	set_unwind(int sig)
{
	g_should_unwind = 1;
}

void	set_unwind_sig(void)
{
	struct sigaction	new_action = {};

	new_action.sa_handler = set_unwind;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = SA_RESTART;
	sigaction(SIGINT, &new_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	set_unwind_sig_norestart(void)
{
	struct sigaction	new_action = {};

	new_action.sa_handler = set_unwind;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGINT, &new_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}
