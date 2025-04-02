#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <readline/readline.h>
#include <signal.h>
#include <stdlib.h>
void	termination_handler(int signum)
{
	ft_eprintf("\n");
	rl_on_new_line();
	rl_replace_line("\n", 0);
	rl_redisplay();
}

void	signal_handling(void)
{
	struct sigaction	new_action;

    new_action.sa_handler = termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, NULL);
}


void	ignore_sig(void)
{
	struct sigaction	new_action;

    new_action.sa_handler = SIG_IGN;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, NULL);
}


void die_sig(int sig) {
	exit(2);
}

void	die_on_sig(void)
{
	struct sigaction	new_action = {};

	new_action.sa_handler = die_sig;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);
}


void set_unwind(int sig) {
	should_unwind = 1;
}
void	set_unwind_sig(void)
{
	struct sigaction	new_action = {};

	new_action.sa_handler = set_unwind;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);
}

