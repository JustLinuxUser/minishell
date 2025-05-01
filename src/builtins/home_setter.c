/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   home_setter.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/01 23:08:33 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 23:18:47 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"
#include <stdlib.h>

int	cd_home(int *e, t_state *state)
{
	char	*home;

	home = env_expand(state, "HOME");
	if (home == NULL)
	{
		ft_eprintf("%s: cd: HOME not set\n", state->context);
		return (1);
	}
	*e = chdir(home);
	return (0);
}
