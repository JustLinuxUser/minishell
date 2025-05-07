/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:32:25 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/07 12:18:31 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
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
	(void)sig;
	g_should_unwind = SIGINT;
}

void	set_unwind_sig(void)
{
	struct sigaction	new_action;

	new_action = (struct sigaction){};
	new_action.sa_handler = set_unwind;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = SA_RESTART;
	sigaction(SIGINT, &new_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	set_unwind_sig_norestart(void)
{
	struct sigaction	new_action;

	new_action = (struct sigaction){};
	new_action.sa_handler = set_unwind;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGINT, &new_action, NULL);
	signal(SIGQUIT, SIG_IGN);
}
