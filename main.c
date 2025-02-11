#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <readline/readline.h>

#include <stdlib.h>
#include <string.h>
#include "libft/dsa/dyn_str.h"
#include "libft/libft.h"
#include "minishell.h"

bool is_space(char c) {
    if (c == ' ' || c == '\t')
        return (true);
    return (false);
}

bool is_special_char(char c) {
    char* specials = ";$'\"<>|&()\n";
    if (ft_strchr(specials, c) || is_space(c)) {
        return true;
    }
    return false;
}

int advance_dquoted(char** str) {
    assert(**str == '\"');
    (*str)++;
    char prev = 0;
    while (**str && (**str != '\"' || prev == '\\')) {
        (*str)++;
    }
    if (**str != '\"')
        return (1);
    (*str)++;
    return (0);
}

int advance_squoted(char** str) {
    assert(**str == '\'');
    (*str)++;
    char prev = 0;
    while (**str && (**str != '\'' || prev == '\\')) {
        (*str)++;
    }
    if (**str != '\'')
        return (1);
    (*str)++;
    return (0);
}

static char* parse_word(t_deque_tt* tokens, char** str) {
    char* start = *str;
    while (1) {
        if (**str == '\\')
            (*str) += 2;
        else if (!is_special_char(**str) || **str == '$') {
            (*str)++;
        } else if (**str == '\'') {
            if (advance_squoted(str))
                return ("squote> ");
        } else if (**str == '"') {
            if (advance_dquoted(str))
                return ("dquote> ");
        } else {
            break;
        }
    }
    deque_tt_push_end(
        tokens, (t_token){.start = start, .len = *str - start, .tt = TT_WORD});
    return (0);
}

typedef struct op_map_s {
    char* str;
    t_tt t;
} op_map_t;

int longest_matching_str(op_map_t* needles, char* haystack) {
    int max_idx = -1;
    int max = -1;
    for (int i = 0; needles[i].str; i++) {
        if ((int)strlen(needles[i].str) > max &&
            strncmp(needles[i].str, haystack, strlen(needles[i].str)) == 0) {
            max_idx = i;
            max = strlen(needles[i].str);
        }
    }
    return (max_idx);
}

void parse_op(t_deque_tt* tokens, char** str) {
    op_map_t operators[] = {
        (op_map_t){"|", TT_PIPE},           (op_map_t){"<<", TT_HEREDOC},
        (op_map_t){"<<-", TT_HEREDOC},      (op_map_t){">>", TT_APPEND},
        (op_map_t){"(", TT_BRACE_LEFT},     (op_map_t){")", TT_BRACE_RIGHT},
        (op_map_t){">", TT_REDIRECT_RIGHT}, (op_map_t){"<", TT_REDIRECT_LEFT},
        (op_map_t){"&&", TT_AND},           (op_map_t){"||", TT_OR},
        (op_map_t){";", TT_SEMICOLON},      (op_map_t){0, TT_END},
    };

    char* start = *str;
    int op_idx = longest_matching_str(operators, *str);
    fflush(stdout);
    assert(op_idx != -1);
    *str += strlen(operators[op_idx].str);
    deque_tt_push_end(tokens, (t_token){.start = start,
                                        .len = *str - start,
                                        .tt = operators[op_idx].t});
}

void parse_env_var(t_deque_tt* tokens, char** str) {
    assert(**str == '$');
    (*str)++;
    char* start = *str;
    while (**str && !is_special_char(**str) && !is_space(**str)) {
        (*str)++;
    }
	deque_tt_push_end(
		tokens,
		(t_token){.start = start, .len = *str - start, .tt = TT_ENVVAR});
}

int parse_dquoted_word(t_deque_tt* tokens, char** str) {
    assert(**str == '"');
    (*str)++;
    char* start = *str;
    char prev = 0;
    while (**str && (**str != '"' || prev == '\\')) {
        prev = **str;
        (*str)++;
    }
    if (**str != '"')
        return (1);
    deque_tt_push_end(
        tokens, (t_token){.start = start, .len = *str - start, .tt = TT_WORD});
    (*str)++;
    return (0);
}

int parse_squoted_word(t_deque_tt* tokens, char** str) {
    assert(**str == '"');
    (*str)++;
    char* start = *str;
    char prev = 0;
    while (**str && (**str != '\'' || prev == '\\')) {
        prev = **str;
        (*str)++;
    }
    if (**str != '\'')
        return (1);
    deque_tt_push_end(
        tokens, (t_token){.start = start, .len = *str - start, .tt = TT_WORD});
    (*str)++;
    return (0);
}

void parse_sep(t_deque_tt* tokens, char** str) {
    char* start = *str;
    assert(is_space(**str));
    while (is_space(**str)) {
        (*str)++;
    }
    deque_tt_push_end(
        tokens, (t_token){.start = start, .len = *str - start, .tt = TT_SEP});
}

// If returns 0, it finished properly, if it returns a ptr, make a prompt
// with that str
char* tokenizer(char* str, t_deque_tt* ret) {
    char* prompt = 0;

    while (*str) {
        if (*str == '$')
            parse_env_var(ret, &str);
        else if (*str == '"') {
            if (parse_dquoted_word(ret, &str))
                prompt = "dquote> ";
        } else if (*str == '\'') {
            if (parse_squoted_word(ret, &str))
                prompt = "squote> ";
        } else if (*str == '\'' || *str == '"' || *str == '$' ||
                   !(is_special_char(*str))) {
            prompt = parse_word(ret, &str);
        } else if (*str == ' ' || *str == '\t') {
            str++;
        } else if (*str == '\n') {
            deque_tt_push_end(
                ret, (t_token){.start = str, .len = 1, .tt = TT_NEWLINE});
            str++;
        } else if (is_space(*str)) {
			parse_sep(ret, &str);
        } else {
            parse_op(ret, &str);
        }
        if (prompt)
            break;
    }
    deque_tt_push_end(ret, (t_token){.tt = TT_END});
    return (prompt);
}

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
        case TT_BSWORD:
            return ("TT_BSWORD");
        case TT_DQUOTE:
            return ("TT_DQUOTE");
        case TT_SQUOTE:
            return ("TT_SQUOTE");
        case TT_ENVVAR:
            return ("TT_ENVVAR");
        case TT_SEP:
            return ("TT_SEP");
        case TT_NONE:
            assert(false);
            break;
    }
    return 0;
}

void print_tokens(t_deque_tt tokens) {
    t_token curr;
    for (int i = 0; i < tokens.len; i++) {
        curr = *deque_tt_idx(&tokens, i);
        printf("%s: >%.*s<\n", tt_to_str(curr.tt), curr.len, curr.start);
    }
}

typedef struct s_exe_cmd {
    char* exe_name;
    char** params;
} t_exe_cmd;

typedef struct s_cmd {
    t_exe_cmd* normal_cmd;
    void* binary_expr;
    bool is_normal_cmd;
} t_cmd;

typedef struct s_braced_expr {
    int piped_commands;
} t_braced_expr;

typedef struct s_binary_expr {
    t_braced_expr* left;
    struct s_binary_expr* right;
    int op;
} t_binary_expr;

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

int main(int argc, char** argv, char** envp) {
    (void)argc;
    (void)argv;
    (void)envp;

    t_state state;
    char* line;
    char* prompt;
    t_deque_tt tt;

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
            line = readline(prompt);
            if (line == 0)
                continue;
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
            prompt = tokenizer(state.prompt.buff, &tt);
        }
        print_tokens(tt);
        t_ast_node node = parse_tokens(&res, &tt);
        if (res == R_OK) {
            print_node(node);
            printf("\n");
            print_ast_dot(node);
            free_ast(node);
        } else {
            prompt = "op> ";
            free_ast(node);
        }
    }

    free(state.prompt.buff);
    free_env(&state.env);
    free(tt.buff);
}
