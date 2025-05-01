/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   export.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/29 23:25:17 by armgonza          #+#    #+#             */
/*   Updated: 2025/05/01 23:23:25 by armgonza         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "builtins.h"

void	parse_export_arg(char *str, char **ident, char **val)
{
	char	*eq;

	eq = ft_strchr(str, '=');
	if (eq)
	{
		*ident = ft_strndup(str, eq - str);
		*val = ft_strdup(eq + 1);
	}
	else
	{
		*ident = ft_strdup(str);
		*val = NULL;
	}
}

static bool	is_valid_ident(char *id)
{
	int	i;

	i = 0;
	if (!is_var_name_p1(id[0]))
		return (false);
	while (id[i] && is_var_name_p2(id[i]))
		i++;
	return (!id[i]);
}

static int	process_arg(t_state *st, t_vec_str av, int i)
{
	char	*id;
	t_env	*e;
	char	*val;

	parse_export_arg(av.buff[i], &id, &val);
	if (is_valid_ident(id))
	{
		if (!val)
		{
			e = env_get(&st->env, id);
			if (e)
				e->exported = true;
			free(id);
		}
		else
			env_set(&st->env, (t_env){true, id, val});
	}
	else
	{
		ft_eprintf("%s: %s: `%s' not valid identifier\n", st->context,
			av.buff[0], av.buff[i]);
		free(id);
		return (free(val), 1);
	}
	return (0);
}

int	builtin_export(t_state *st, t_vec_str av)
{
	size_t	i;
	int		status;

	i = 1;
	status = 0;
	if (av.len == 1)
		return (builtin_env(st, av));
	while (i < av.len && !status)
	{
		status = process_arg(st, av, i);
		i++;
	}
	return (status);
}
