#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <readline/readline.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "libft/dsa/dyn_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"

char* tt_to_str(t_tt tt) {
    switch (tt) {
        case TT_END:
            return ("TT_END");
        case TT_WORD:
            return ("TT_WORD");
        case TT_REDIRECT_LEFT:
            return ("TT_REDIRECT_LEFT");
        case TT_REDIRECT_RIGHT:
            return ("TT_REDIRECT_RIGHT");
        case TT_APPEND:
            return ("TT_APPEND");
        case TT_PIPE:
            return ("TT_PIPE");
        case TT_BRACE_LEFT:
            return ("TT_BRACE_LEFT");
        case TT_BRACE_RIGHT:
            return ("TT_BRACE_RIGHT");
        case TT_OR:
            return ("TT_OR");
        case TT_AND:
            return ("TT_AND");
        case TT_SEMICOLON:
            return ("TT_SEMICOLON");
        case TT_HEREDOC:
            return ("TT_HEREDOC");
        case TT_NEWLINE:
            return ("TT_NEWLINE");
        case TT_SQWORD:
            return ("TT_QWORD");
        case TT_DQWORD:
            return ("TT_DQWORD");
        case TT_ENVVAR:
            return ("TT_ENVVAR");
        case TT_DQENVVAR:
            return ("TT_DQENVVAR");
        case TT_NONE:
            assert(false);
            break;
    }
    return 0;
}

void print_tokens(t_deque_tt tokens) {
    t_token curr;
    printf("------- PRINTING TOKENS --------\n");
    for (int i = 0; i < tokens.len; i++) {
        curr = *deque_tt_idx(&tokens, i);
        printf("%s: >%.*s<\n", tt_to_str(curr.tt), curr.len, curr.start);
    }
    printf("------- DONE --------\n");
}

bool replace_escaped_newlines(t_dyn_str* input) {
    t_dyn_str out;
    int i;
    bool ret;
    ret = false;
    bool backslash;

    dyn_str_init(&out);
    i = -1;
    backslash = false;
    while (input->buff[++i]) {
        if (input->buff[i] == '\\') {
            backslash = true;
            continue;
        }
        if (backslash) {
            if (input->buff[i] != '\n') {
                dyn_str_push(&out, '\\');
                dyn_str_push(&out, input->buff[i]);
            } else
                ret = true;
            backslash = false;
            continue;
        }
        dyn_str_push(&out, input->buff[i]);
    }
    free(input->buff);
    *input = out;
    return (ret);
}

void termination_handler(int signum) {
    ft_eprintf("\n");
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

void signal_handling(void) {
    struct sigaction new_action;

    /* Set up the structure to specify the new action. */
    new_action.sa_handler = termination_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGINT, &new_action, NULL);
}

int main(int argc, char** argv, char** envp) {
    (void)argc;
    (void)argv;
    (void)envp;

    t_state state;
    char* line;
    char* prompt;
    t_deque_tt tt;
    signal_handling();

    deque_tt_init(&tt, 100);
    dyn_str_init(&state.prompt);
    state.env = env_to_vec_env(envp);

    printf("env: %s\n", vec_env_idx(&state.env, 0)->key);
    printf("path: %s\n", env_get(&state.env, "PATH")->value);

    prompt = "prompt> ";
    t_res res = R_MoreInput;
    while (res == R_MoreInput) {
        while (prompt) {
            tt.len = 0;
            tt.start = 0;
            tt.end = 0;
            line = readline(prompt);
            if (line == 0) {
                printf("Ctrl-D\n");
                res = R_FatalError;
                break;
            }
            dyn_str_pushstr(&state.prompt, line);
            free(line);
            if (state.prompt.len == 0)
                continue;
            if (state.prompt.buff[state.prompt.len - 1] == '\\') {
                state.prompt.len--;
                prompt = ">";
                continue;
            }
            dyn_str_push(&state.prompt, '\n');
            printf("prompt: %s\n", state.prompt.buff);
            prompt = tokenizer(state.prompt.buff, &tt);
        }
        if (tt.len) {
            print_tokens(tt);
            t_ast_node node = parse_tokens(&res, &tt);
            if (res == R_OK) {
                print_node(node);
                printf("\n");
                print_ast_dot(node);
                state.tree = node;
                execute_top_level(&state);
                free_ast(node);
            } else {
                prompt = "op> ";
                free_ast(node);
            }
        } else {
            break;
        }
    }

    free(state.prompt.buff);
    free_env(&state.env);
    free(tt.buff);
    return (res);
}
