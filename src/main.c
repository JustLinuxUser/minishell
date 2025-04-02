#include <unistd.h>
#include <assert.h>
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

// Read fd untill the end, and append the results to *ret
void dyn_str_append_fd(int fd, t_dyn_str *ret)
{
	char buff[1024];
	int len;

	while (1)
	{
		len = read(fd, buff, sizeof(buff));
		if (len == 0)
			break;
		if (len > 0)
			dyn_str_pushnstr(ret, buff, len);
		else
			critical_error_errno();
	}
}

char *getpid_hack() {
	int fd;
	t_dyn_str file;
	char *ret;
	char **temp;

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

t_dyn_str getcwd_dyn_str()
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

t_dyn_str new_prompt(t_parser *parser)
{
	t_dyn_str ret;

	dyn_str_init(&ret);
	t_tt curr;
	for (size_t i = 0; i < parser->parse_stack.len; i++) {
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
			continue;
		dyn_str_pushstr(&ret, " ");
	}
	ret.buff[ret.len - 1] = '>';
	dyn_str_pushstr(&ret, " ");
	return (ret);
}

void get_input(t_state *state, char *prompt, t_deque_tt *tt) {
	int			stat;

	while (prompt) {
		tt->len = 0;
		tt->start = 0;
		tt->end = 0;
		stat = buff_readline(&state->readline_buff, &state->input, prompt);
		free(prompt);
		if (stat == 0) {
			state->should_exit = true;
			break;
		} else if (stat == 2)
		{
			state->should_reset = 1;
			break;
		}
		if (state->input.len == 0)
			break;
		if (state->input.buff[state->input.len - 1] == '\\') {
			state->input.len--;
			prompt = ft_strdup("> ");
			continue;
		}
		dyn_str_push(&state->input, '\n');
		prompt = tokenizer(state->input.buff, tt);
		if (prompt)
			prompt = ft_strdup(prompt);
	}
}

void manage_history(t_state *state) {
	if (state->readline_buff.cursor > 1)
	{

		char *temp = ft_strndup(state->readline_buff.buff.buff, state->readline_buff.cursor - 1);
		add_history(temp);
		free(temp);
	}
	buff_readline_reset(&state->readline_buff);
}

void execute_line(t_state *state)
{
    t_deque_tt tt;
	char *prompt;

    prompt = ft_strdup("prompt> ");

    deque_tt_init(&tt, 100);
    t_parser parser = {.res = RES_MoreInput,
		.prog_name = state->argv[0]};
    while (parser.res == RES_MoreInput) {
		parser.parse_stack.len = 0;
		get_input(state, prompt, &tt);
		if (state->should_exit)
			break;
		if (state->should_reset)
		{
			state->should_reset = false;
			break;
		}
        if (tt.len) {
            state->tree = parse_tokens(&parser, &tt);
            if (parser.res == RES_OK) {
				free(parser.parse_stack.buff);
				parser.parse_stack = (t_vec_int){};
				print_ast_dot(state->tree);
                execute_top_level(state);
				manage_history(state);
				if (should_unwind)
				{
					free(state->last_cmd_status);
					state->last_cmd_status = ft_itoa(SYNTAX_ERR);
				}
            } else if (parser.res == RES_MoreInput) {
                prompt = new_prompt(&parser).buff;
            } else if (parser.res == RES_FatalError) {
				free(state->last_cmd_status);
				state->last_cmd_status = ft_itoa(CANCELED);
			}
			free_ast(state->tree);
        } else {
			manage_history(state);
            break;
        }
    }
	free (parser.parse_stack.buff);
    free(tt.buff);
}



void free_redirects(t_vec_redir *v)
{
	size_t	i;
	t_redir c;
	
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

void free_all_state(t_state state)
{
	free(state.input.buff);
	free (state.last_cmd_status);
	free (state.pid);
	free (state.cwd.buff);
	free (state.readline_buff.buff.buff);
	free_redirects(&state.redirects);
}

int main(int argc, char** argv, char** envp)
{
    (void)argc;
    t_state state;

	if (!isatty(1))
		return (0);
	ignore_sig();

	state = (t_state){0};
	state.pid = getpid_hack();

    state.env = env_to_vec_env(envp);
	state.cwd = getcwd_dyn_str();

	state.argv = argv;
	state.last_cmd_status = ft_strdup("0");

	while (!state.should_exit)
	{
		dyn_str_init(&state.input);
		should_unwind = 0;
		execute_line(&state);
		free_redirects(&state.redirects);
		free(state.input.buff);
	}

    free_env(&state.env);
	int status = ft_atoi(state.last_cmd_status);
	free (state.last_cmd_status);
	free (state.pid);
	free (state.cwd.buff);
	free (state.readline_buff.buff.buff);
    return (status);
}
