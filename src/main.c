/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 09:39:34 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/12 19:45:33 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <stdbool.h>
#include <stdio.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "libft/libft.h"

#include "libft/dsa/dyn_str.h"

char	*getpid_hack(void)
{
	int			fd;
	t_dyn_str	file;
	char		*ret;
	char		**temp;

	fd = open("/proc/self/stat", O_RDONLY);
	if (fd < 0)
	{
		warning_error("Cannot get PID.");
		return (0);
	}
	dyn_str_init(&file);
	dyn_str_append_fd(fd, &file);
	temp = ft_split(file.buff, ' ');
	free(file.buff);
	ret = ft_strdup(temp[0]);
	free_tab(temp);
	return (ret);
}

t_dyn_str	getcwd_dyn_str(void)
{
	t_dyn_str	ret;

	dyn_str_init(&ret);
	ret.buff = getcwd(0, 0);
	if (!ret.buff)
		critical_error_errno();
	ret.len = ft_strlen(ret.buff);
	ret.cap = ret.len;
	return (ret);
}

t_dyn_str	prompt_more_input(t_parser *parser)
{
	t_dyn_str	ret;
	t_tt		curr;
	size_t		i;

	i = 0;
	dyn_str_init(&ret);
	while (i < parser->parse_stack.len)
	{
		curr = vec_int_idx(&parser->parse_stack, i);
		if (curr == TT_BRACE_LEFT)
			dyn_str_pushstr(&ret, "subsh");
		else if (curr == TT_PIPE)
			dyn_str_pushstr(&ret, "pipe");
		else if (curr == TT_AND)
			dyn_str_pushstr(&ret, "cmdand");
		else if (curr == TT_OR)
			dyn_str_pushstr(&ret, "cmdor");
		else
		{
			i++;
			continue ;
		}
		dyn_str_pushstr(&ret, " ");
		i++;
	}
	ret.buff[ret.len - 1] = '>';
	dyn_str_pushstr(&ret, " ");
	return (ret);
}

t_dyn_str	prompt_normal(t_state *state)
{
	t_dyn_str	ret;
	dyn_str_init(&ret);
	dyn_str_push(&ret, RL_PROMPT_START_IGNORE);
	if (ft_strcmp(state->last_cmd_status, "0") == 0)
		dyn_str_pushstr(&ret, ANSI_GREEN);
	else
		dyn_str_pushstr(&ret, ANSI_RED);
	dyn_str_pushstr(&ret, PROMPT);
	dyn_str_pushstr(&ret, ANSI_RESET);
	dyn_str_push(&ret, RL_PROMPT_END_IGNORE);
	dyn_str_pushstr(&ret, RL_SPACER_1);
	dyn_str_pushstr(&ret, RL_SPACER_1);
	return (ret);
}

bool	readline_cmd(t_state *state, char *prompt, t_deque_tt *tt)
{
	int			stat;

	deque_tt_clear(tt);
	stat = buff_readline(state, &state->input, prompt);
	free(prompt);
	if (stat == 0 || stat == 2 || !state->input.len)
	{
		if (stat == 0)
			state->should_exit = true;
		else if (stat == 2)
			state->should_reset = 1;
		return (true);
	}
	return (false);
}

void	get_more_tokens(t_state *state, char *prompt, t_deque_tt *tt)
{
	while (prompt)
	{
		if (readline_cmd(state, prompt, tt))
			break ;
		if (dyn_str_ends_with_str(&state->input, "\\"))
		{
			dyn_str_pop(&state->input);
			prompt = ft_strdup("> ");
			continue ;
		}
		dyn_str_push(&state->input, '\n');
		prompt = tokenizer(state->input.buff, tt);
		if (prompt)
			prompt = ft_strdup(prompt);
	}
}

void	manage_history(t_state *state)
{
	char	*temp;

	if (state->readline_buff.cursor > 1)
	{
		temp = ft_strndup(state->readline_buff.buff.buff,
				state->readline_buff.cursor - 1);
		add_history(temp);
		free(temp);
	}
	buff_readline_reset(&state->readline_buff);
}

bool	try_parse_tokens(t_state *state, t_parser *parser, t_deque_tt *tt, char **prompt)
{
	if (!tt->len)
	{
		buff_readline_reset(&state->readline_buff);
		return (false);
	}
	parser->parse_stack.len = 0;
	state->tree = parse_tokens(parser, tt);
	if (parser->res == RES_OK)
		return (true);
	else if (parser->res == RES_MoreInput)
	{
		*prompt = prompt_more_input(parser).buff;
	}
	else if (parser->res == RES_FatalError)
	{
		free(state->last_cmd_status);
		state->last_cmd_status = ft_itoa(SYNTAX_ERR);
	}
	free_ast(&state->tree);
	return (true);
}

void	set_cmd_status(t_state *state, char *new_status)
{
	free(state->last_cmd_status);
	state->last_cmd_status = new_status;
}

void	execute_tree(t_state *state)
{
	print_ast_dot(state->tree);
	execute_top_level(state);
	free_ast(&state->tree);
	if (g_should_unwind)
	{
		free(state->last_cmd_status);
		state->last_cmd_status = ft_itoa(CANCELED);
	}
}

void	parse_and_execute_input(t_state *state)
{
	t_deque_tt	tt;
	char		*prompt;
	t_parser	parser;

	parser = (t_parser){.res = RES_MoreInput,
		.prog_name = state->context};
	prompt = prompt_normal(state).buff;
	deque_tt_init(&tt, 100);
	while (parser.res == RES_MoreInput)
	{
		get_more_tokens(state, prompt, &tt);
		if (state->should_exit)
			break ;
		if (g_should_unwind)
			set_cmd_status(state, ft_itoa(CANCELED));
		if (!try_parse_tokens(state, &parser, &tt, &prompt))
			break ;
	}
	if (parser.res == RES_OK)
	{
		execute_tree(state);
		manage_history(state);
	}
	manage_history(state);
	free (parser.parse_stack.buff);
	parser.parse_stack = (t_vec_int){};
	free(tt.buff);
}

void	free_redirects(t_vec_redir *v)
{
	size_t	i;
	t_redir	c;

	i = 0;
	while (i < v->len)
	{
		c = v->buff[i];
		if (c.should_delete)
			unlink(c.fname);
		free(c.fname);
		i++;
	}
	free(v->buff);
	vec_redir_init(v);
}

void	free_all_state(t_state *state)
{
	free(state->input.buff);
	state->input = (t_dyn_str){};
	free(state->last_cmd_status);
	free(state->pid);
	free(state->context);
	free(state->base_context);
	state->context = 0;
	state->base_context = 0;
	free(state->readline_buff.buff.buff);
	free_redirects(&state->redirects);
	free_ast(&state->tree);
}

void	init_setup(t_state *state, char **argv, char **envp)
{
	ignore_sig();
	*state = (t_state){0};
	state->pid = getpid_hack();
	state->env = env_to_vec_env(envp);
	state->context = ft_strdup(argv[0]);
	state->base_context = ft_strdup(argv[0]);
	state->last_cmd_status = ft_strdup("0");
	if (argv[1] && ft_strcmp(argv[1], "-c") == 0)
	{
		if (!argv[2])
		{
			ft_eprintf("%s: -c: option requires an argument\n", state->base_context);
			free_all_state(state);
			exit(SYNTAX_ERR);
		}
		dyn_str_pushstr(&state->readline_buff.buff, argv[2]);
		dyn_str_push(&state->readline_buff.buff, '\n');
		buff_readline_update(&state->readline_buff);
		state->readline_buff.no_readline = true;
		state->readline_buff.should_update_context = true;
	}
	else if (argv[1])
	{
		int fd = open(argv[1], O_RDONLY);
		if (fd < 0)
		{
			ft_eprintf("%s: %s: %s\n", state->base_context, argv[1], strerror(errno));
			free_all_state(state);
			if (errno == EISDIR)
				exit(127);
			exit(EXE_BASE_ERROR + errno);
		}
		dyn_str_append_fd(fd, &state->readline_buff.buff);
		dyn_str_push(&state->readline_buff.buff, '\n');
		buff_readline_update(&state->readline_buff);
		state->readline_buff.no_readline = true;
		state->readline_buff.should_update_context = true;
		free(state->base_context);
		free(state->context);
		state->base_context = ft_strdup(argv[1]);
		state->context = ft_strdup(argv[1]);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_state	state;
	int		status;

	(void)argc;
	if (!isatty(1))
		return (0);
	init_setup(&state, argv, envp);
	while (!state.should_exit)
	{
		dyn_str_init(&state.input);
		g_should_unwind = 0;
		parse_and_execute_input(&state);
		free_redirects(&state.redirects);
		free_ast(&state.tree);
		free(state.input.buff);
		state.input = (t_dyn_str){};
	}
	status = ft_atoi(state.last_cmd_status);
	free_env(&state.env);
	free_all_state(&state);
	return (status);
}
