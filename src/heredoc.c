/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 13:59:13 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/01 00:42:42 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

// returns writable fd
int ft_mktemp(t_state *state, t_ast_node *node)
{
	t_redir ret;
	char *temp;
	int wr_fd;
	t_dyn_str fname;

	ret = (t_redir){.direction_in = true, .should_delete = true};
	dyn_str_init(&fname);
	dyn_str_pushstr(&fname, "/tmp/heredoc_");
	if (state->pid)
		dyn_str_pushstr(&fname, state->pid);
	dyn_str_pushstr(&fname, "_");
	temp = ft_itoa(state->heredoc_idx++);

	dyn_str_pushstr(&fname, temp);
	ret.fname = fname.buff;
	free(temp);
	wr_fd = open(ret.fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (wr_fd < 0)
		critical_error_errno();
	ret.fd = open(ret.fname, O_RDONLY);
	if (ret.fd < 0)
		critical_error_errno();
	vec_redir_push(&state->heredocs, ret);
	node->heredoc_idx = state->heredocs.len - 1;
	node->is_heredoc = true;
	return (wr_fd);
}

char *first_non_tab(char *line)
{
	while (*line == '\t')
		line++;
	return (line);
}

int env_len (char *line)
{
	int	len;

	len = 0;
	if (is_var_name_p1(line[len]))
		len++;
	else
		return (len);
	while (is_var_name_p2(line[len]))
		len++;
	return (len);
}
bool is_escapable(char c)
{
	if (c == '\\' || c == '$')
		return true;
	return false;
}

void expand_line(t_state *state, t_dyn_str *full_file, char *line)
{
	int	i;
	int len;
	bool bs;

	i = 0;
	bs = 0;
	while (line[i])
	{
		if (bs)
		{
			if (is_escapable(line[i]))
				dyn_str_push(full_file, line[i]);
			else if (line[i] != '\n')
			{
				dyn_str_push(full_file, '\\');
				dyn_str_push(full_file, line[i]);
			}
			i++;
			bs = false;
			continue;
		}
		if (line[i] == '$')
		{
			i++;
			len = env_len(line + i);
			if (len)
			{
				char *env = env_expand_n(state, line + i, len);
				if (env)
					dyn_str_pushstr(full_file, env);
			}
			else 
				dyn_str_push(full_file, line[i]);
			i += len;
			continue;
		}
		else if (line[i] == '\\')
			bs = true;
		else
			dyn_str_push(full_file, line[i]);
		i++;
	}
}

bool is_separator(char *input, char *sep)
{
	int len_input;
	int len_sep;
	if (ft_strcmp(input, sep) == 0)
		return (true);
	len_input = ft_strlen(input);
	len_sep = ft_strlen(sep);
	if (len_input > len_sep) {
		if (input[len_input - 1 - len_sep] == '\n' &&
			ft_strcmp(&input[len_input - len_sep], sep) == 0)
			return (true);
	}
	return (false);
}

typedef struct s_heredoc_req {
	t_dyn_str full_file;
	bool finished;
	char *sep;
	bool expand;
	bool remove_tabs;
} t_heredoc_req;

void process_line(t_state *state, t_heredoc_req *req);

bool ends_in_backslash(char *line)
{
	int	len;
	int bs_count;

	bs_count = 0;
	len = ft_strlen(line);

	if (len == 0)
		return false;
	while (line[len - 1] == '\\')
	{
		bs_count++;
		len--;
	}
	if (bs_count % 2 == 0)
		return false;
	return (true);
}

void bg_read_heredoc(t_state *state, t_heredoc_req *req, int outfd)
{
	int start_cursor;

	start_cursor = state->readline_buff.cursor;
	while (!req->finished) {
		process_line(state, req);
	}
	if (state->readline_buff.buff.buff)
		assert(write_to_file(&state->readline_buff.buff.buff[start_cursor], outfd) == 0);
	exit (0);
}

// should brake
void process_line(t_state *state, t_heredoc_req *req)
{
	t_dyn_str alloc_line;
	int stat;
	char *line;

	dyn_str_init(&alloc_line);
	stat = buff_readline(&state->readline_buff, &alloc_line, "heredoc> ");
	if (stat == 0)
	{
		req->finished = true;
		return;
	}
	if ((req->full_file.len == 0
			|| req->full_file.buff[req->full_file.len - 1] == '\n')
		&& ft_strcmp(alloc_line.buff, req->sep) == 0)
	{
		free(alloc_line.buff);
		req->finished = true;
		return;
	}
	dyn_str_push(&alloc_line, '\n');
	if (req->remove_tabs)
		line = first_non_tab(alloc_line.buff);
	else 
		line = alloc_line.buff;
	if (req->expand)
		expand_line(state, &req->full_file, line);
	else
		dyn_str_pushstr(&req->full_file, line);
	free(alloc_line.buff);
}

void get_more_input_heredoc(t_state *state, t_heredoc_req *req)
{
	int pp[2];

	if (pipe(pp))
		critical_error_errno();
	int pid = fork();
	if (pid == 0) {
		die_on_sig();
		close(pp[0]);
		bg_read_heredoc(state, req, pp[1]);
	} else if (pid < 0) {
		critical_error_errno();
	} else  {
		ignore_sig();
		close(pp[1]);
		dyn_str_append_fd(pp[0], &state->readline_buff.buff);
		buff_readline_update(&state->readline_buff);
		close(pp[0]);
		wait(0);
	}
}

void write_heredoc(t_state *state, int wr_fd, t_heredoc_req *req)
{
	printf("remove_tabs: %i\n", req->remove_tabs);
	while (state->readline_buff.has_line && !req->finished) {
		process_line(state, req);
	}
	if (!req->finished)
		get_more_input_heredoc(state, req);
	while (state->readline_buff.has_line && !req->finished) {
		process_line(state, req);
	}
	if (req->full_file.len)
		assert(write_to_file(req->full_file.buff, wr_fd) == 0);
	free(req->full_file.buff);
}
//
bool contains_quotes(t_ast_node node)
{
    if (node.node_type == AST_TOKEN) {
        if (node.token.tt == TT_DQENVVAR || node.token.tt == TT_DQWORD ||
            node.token.tt == TT_SQWORD) {
            return (true);
        }
    }
	for (size_t i = 0; i < node.children.len; i++)
	{
		if (contains_quotes(node.children.buff[i]))
			return (true);
	}
	return (false);
}

int gather_heredocs(t_state* state, t_ast_node* node) {
    size_t i;

    i = 0;
    while (i < node->children.len) {
        gather_heredocs(state, &node->children.buff[i]);
		i++;
    }
    if (node->node_type == AST_REDIRECT)
	{
        assert(node->children.len >= 1);
        if (node->children.buff[0].token.tt == TT_HEREDOC)
		{
			int wr_fd = ft_mktemp(state, node);
			char *sep = expand_word_single(state, &node->children.buff[1]);
			assert(sep);
			t_heredoc_req req = {
				.sep = sep,
				.expand = !contains_quotes(node->children.buff[1]),
				.remove_tabs =
					ft_strncmp(node->children.buff[0].token.start, "<<-", 3) == 0};
			write_heredoc(state, wr_fd, &req);
			free(sep);
        }
    }
    return (0);
}
