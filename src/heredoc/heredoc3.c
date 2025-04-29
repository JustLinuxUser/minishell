/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 06:53:56 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/28 13:00:45 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

bool	is_escapable(char c)
{
	if (c == '\\' || c == '$')
		return (true);
	return (false);
}

void	expand_dolar(t_state *state, int *i, t_dyn_str *full_file, char *line)
{
	int		len;
	char	*env;

	(*i)++;
	len = env_len(line + *i);
	if (len)
	{
		env = env_expand_n(state, line + *i, len);
		if (env)
			dyn_str_pushstr(full_file, env);
	}
	else
		dyn_str_push(full_file, line[*i]);
	*i += len;
}

void	expand_bs(int *i, t_dyn_str *full_file, char *line)
{
	if (is_escapable(line[*i]))
		dyn_str_push(full_file, line[*i]);
	else if (line[*i] != '\n')
	{
		dyn_str_push(full_file, '\\');
		dyn_str_push(full_file, line[*i]);
	}
	(*i)++;
}

void	expand_line(t_state *state, t_dyn_str *full_file, char *line)
{
	int		i;
	bool	bs;

	i = 0;
	bs = 0;
	while (line[i])
	{
		if (bs)
		{
			expand_bs(&i, full_file, line);
			bs = false;
			continue ;
		}
		if (line[i] == '$')
		{
			expand_dolar(state, &i, full_file, line);
			continue ;
		}
		else if (line[i] == '\\')
			bs = true;
		else
			dyn_str_push(full_file, line[i]);
		i++;
	}
}
