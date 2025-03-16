#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "libft/libft.h"
#include "minishell.h"

char* node_name(t_ast_t tn) {
    switch (tn) {
        case AST_COMMAND_PIPELINE:
            return ("AST_COMMAND_PIPELINE");
        case AST_AND:
            return ("AST_AND");
        case AST_OR:
            return ("AST_OR");
        case AST_REDIRECT:
            return ("AST_REDIRECT");
        case AST_WORD:
            return ("AST_CMD_WORD");
        case AST_TOKEN:
            return ("AST_TOKEN");
        case AST_SIMPLE_LIST:
            return ("AST_SIMPLE_LIST");
        case AST_SIMPLE_COMMAND:
            return ("AST_SIMPLE_COMMAND");
        case AST_SUBSHELL:
            return ("AST_SUBSHELL");
        case AST_COMPOUND_LIST:
            return ("AST_COMPOUND_LIST");
        case AST_COMMAND:
            return ("AST_COMMAND");
        case AST_ASSIGNMENT_WORD:
            return ("AST_ASSIGNMENT_WORD");
    }
    return 0;
}

bool is_simple_cmd_token(t_tt tt) {
    switch (tt) {
        case TT_REDIRECT_LEFT:
        case TT_WORD:
        case TT_REDIRECT_RIGHT:
        case TT_APPEND:
        case TT_HEREDOC:
            return true;
        default:
            break;
    }
    return false;
}

void print_node(t_ast_node node) {
    printf(" (%s", node_name(node.node_type));
    if (node.node_type == AST_TOKEN) {
        printf(" %s: >%.*s<", tt_to_str(node.token.tt), node.token.len,
               node.token.start);
    }
    for (size_t i = 0; i < node.children.len; i++) {
        print_node(*vec_nd_idx(&node.children, i));
    }
    printf(")");
}
void print_token_str(t_ast_node node, FILE* out) {
    for (int i = 0; i < node.token.len; i++) {
        char c = node.token.start[i];
        if (c == '\\')
            fprintf(out, "\\\\");
        else if (c == '"')
            fprintf(out, "\\\"");
        else if (c == '\'')
            fprintf(out, "\\'");
        else
            fprintf(out, "%c", c);
    }
}

void print_dot_node(t_ast_node node, int id, FILE* out) {
    fprintf(out, "	n%i [label=\"%s", id, node_name(node.node_type));
    if (node.node_type == AST_TOKEN) {
        fprintf(out, " %s: >", tt_to_str(node.token.tt));
        print_token_str(node, out);
        fprintf(out, "<");
    }
    fprintf(out, "\"];\n");
    for (size_t i = 0; i < node.children.len; i++) {
        int r = rand();
        fprintf(out, "	n%i -> n%i;\n", id, r);
        print_dot_node(*vec_nd_idx(&node.children, i), r, out);
    }
}

void print_ast_dot(t_ast_node node) {
    FILE* out = fopen("out", "w");
    fprintf(out, "digraph G {\n");
    print_dot_node(node, 0, out);
    fprintf(out, "}\n");
    fclose(out);
}

void traverce_ast(t_ast_node* node, void (*f)(t_ast_node* node)) {
    size_t i;

    i = 0;
    while (i < node->children.len) {
        traverce_ast(vec_nd_idx(&node->children, i), f);
        i++;
    }
    f(node);
}

void free_node(t_ast_node* node) {
    free(node->children.buff);
}

void free_ast(t_ast_node node) {
    traverce_ast(&node, free_node);
}

bool is_redirect(t_tt tt) {
    switch (tt) {
        case TT_REDIRECT_LEFT:
        case TT_REDIRECT_RIGHT:
        case TT_APPEND:
        case TT_HEREDOC:
            return true;
        default:
            break;
    }
    return (false);
}
bool is_compund_list_op(t_tt t) {
    switch (t) {
        case TT_SEMICOLON:
        case TT_OR:
        case TT_AND:
        case TT_NEWLINE:
            return (true);
        default:
            break;
    }
    return false;
}

t_ast_node parse_pipeline(t_res* res, t_deque_tt* tokens);
t_ast_node unexpected(t_res* res, t_ast_node ret, t_deque_tt* tokens) {
    printf("andriish: syntax error near unexpected token %s\n",
           tt_to_str(deque_tt_peek(tokens).tt));
    *res = R_FatalError;
    assert(false);
    return (ret);
}

t_ast_node parse_compound_list(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMPOUND_LIST};
    vec_nd_push(&ret.children, parse_pipeline(res, tokens));
    if (*res != R_OK)
        return (ret);
    while (is_compund_list_op(deque_tt_peek(tokens).tt)) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        if (*res != R_OK)
            return (ret);
        if (ret.children.buff[ret.children.len - 1].token.tt == TT_SEMICOLON &&
            deque_tt_peek(tokens).tt == TT_BRACE_RIGHT) {
            return (ret);
        }
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (unexpected(res, ret, tokens));
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

t_ast_node parse_subshell(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_SUBSHELL};
    if (deque_tt_peek(tokens).tt != TT_BRACE_LEFT)
        return (unexpected(res, ret, tokens));
    deque_tt_pop_start(tokens);

    vec_nd_push(&ret.children, parse_compound_list(res, tokens));
    if (*res != R_OK) {
        return (ret);
    }
    if (deque_tt_peek(tokens).tt != TT_BRACE_RIGHT)
        return (unexpected(res, ret, tokens));
    deque_tt_pop_start(tokens);
    return (ret);
}

t_ast_node parse_word(t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_WORD};
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN,
                             .token = deque_tt_pop_start(tokens)});
    return (ret);
}

t_ast_node parse_redirect(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_REDIRECT};
    t_token t = deque_tt_pop_start(tokens);

    if (!is_redirect(t.tt)) {
        return (unexpected(res, ret, tokens));
    }
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN, .token = t});
    if (deque_tt_peek(tokens).tt != TT_WORD)
        return (unexpected(res, ret, tokens));
    vec_nd_push(&ret.children, parse_word(tokens));
    return (ret);
}

t_ast_node parse_simple_command(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_SIMPLE_COMMAND};

    t_tt next;
    next = deque_tt_peek(tokens).tt;
    if (!is_simple_cmd_token(next)) {
        return (unexpected(res, ret, tokens));
    }
    while (1) {
        next = deque_tt_peek(tokens).tt;
        if (next == TT_WORD)
            vec_nd_push(&ret.children, parse_word(tokens));
        else if (is_redirect(next)) {
            vec_nd_push(&ret.children, parse_redirect(res, tokens));
            if (*res != R_OK)
                return (ret);
        } else
            break;
    }

    return (ret);
}

t_ast_node parse_command(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_COMMAND};
    t_tt next;
    next = deque_tt_peek(tokens).tt;
    if (next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_subshell(res, tokens));
        if (*res != R_OK)
            return (ret);
        while (is_redirect(deque_tt_peek(tokens).tt)) {
            vec_nd_push(&ret.children, parse_redirect(res, tokens));
            if (*res != R_OK)
                return (ret);
        }
    } else {
        vec_nd_push(&ret.children, parse_simple_command(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

t_ast_node parse_pipeline(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_COMMAND_PIPELINE};

    vec_nd_push(&ret.children, parse_command(res, tokens));
    if (*res != R_OK)
        return (ret);
    while (deque_tt_peek(tokens).tt == TT_PIPE) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (*res = R_MoreInput, ret);
        vec_nd_push(&ret.children, parse_command(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    return (ret);
}

bool is_simple_list_op(t_tt t) {
    switch (t) {
        case TT_SEMICOLON:
        case TT_OR:
        case TT_AND:
            return (true);
        default:
            break;
    }
    return false;
}

t_ast_node parse_simple_list(t_res* res, t_deque_tt* tokens) {
    t_ast_node ret = {.node_type = AST_SIMPLE_LIST};

    t_tt next = deque_tt_peek(tokens).tt;
    if (is_simple_cmd_token(next) || next == TT_BRACE_LEFT) {
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    while (is_simple_list_op(deque_tt_peek(tokens).tt)) {
        vec_nd_push(&ret.children,
                    (t_ast_node){.node_type = AST_TOKEN,
                                 .token = deque_tt_pop_start(tokens)});
        if (*res != R_OK)
            return (ret);
        if (ret.children.buff[ret.children.len - 1].token.tt == TT_SEMICOLON &&
            deque_tt_peek(tokens).tt == TT_END) {
            return (ret);
        }
        while (deque_tt_peek(tokens).tt == TT_NEWLINE)
            deque_tt_pop_start(tokens);
        if (deque_tt_peek(tokens).tt == TT_END)
            return (*res = R_MoreInput, ret);
        vec_nd_push(&ret.children, parse_pipeline(res, tokens));
        if (*res != R_OK)
            return (ret);
    }
    if (deque_tt_peek(tokens).tt == TT_NEWLINE)
        deque_tt_pop_start(tokens);
    else
        return (unexpected(res, ret, tokens));
    return (ret);
}

bool can_flatten(t_ast_t t) {
    switch (t) {
        break;
        case AST_SIMPLE_LIST:
        case AST_COMMAND_PIPELINE:
        case AST_COMMAND:
        case AST_COMPOUND_LIST:
            return true;
        default:
            break;
    }
    return (false);
}

void flatten(t_ast_node* node) {
    void* temp;

    for (size_t i = 0; i < node->children.len; i++) {
        flatten(&node->children.buff[i]);
    }
    if (node->children.len == 1 && can_flatten(node->node_type)) {
        temp = node->children.buff;
        *node = node->children.buff[0];
        free(temp);
    }
}

t_ast_node create_subtoken_node(t_token t,
                                int offset,
                                int end_offset,
                                t_tt tt) {
    t_ast_node ret;

    ret = (t_ast_node){.token =
                           {
                               .len = end_offset - offset,
                               .start = t.start + offset,
                               .tt = tt,
                           },
                       .node_type = AST_TOKEN};
    return (ret);
}

//[a-zA-Z_]
bool is_var_name_p1(char c)
{
	if (ft_isalpha(c) || c == '_')
		return (true);
	return (false);
}

//[a-zA-Z0-9_]
bool is_var_name_p2(char c)
{
	if (ft_isalnum(c) || c == '_')
		return (true);
	return (false);
}

void reparse_envvar(t_ast_node* ret, int* i, t_token t, t_tt tt) {
    assert(t.start[(*i)++] == '$');

    int prev_start = *i;
	if (*i < t.len && is_var_name_p1(t.start[*i]))
		(*i)++;
    while (*i < t.len && is_var_name_p2(t.start[*i])) {
        (*i)++;
    }
    if (prev_start == *i + 1) {
        vec_nd_push(&ret->children,
                    create_subtoken_node(t, prev_start - 1, *i, TT_WORD));
    } else {
        vec_nd_push(&ret->children,
                    create_subtoken_node(t, prev_start, *i, tt));
    }
}

void reparse_dquote(t_ast_node* ret, int* i, t_token t) {
    assert(t.start[(*i)++] == '"');

    int prev_start = *i;
    while (*i < t.len && t.start[*i] != '"') {
        if (t.start[*i] == '$') {
            vec_nd_push(&ret->children,
                        create_subtoken_node(t, prev_start, *i, TT_DQWORD));
            reparse_envvar(ret, i, t, TT_DQENVVAR);
            prev_start = *i;
            continue;
        }
        (*i)++;
    }
    vec_nd_push(&ret->children,
                create_subtoken_node(t, prev_start, *i, TT_DQWORD));
    assert(t.start[(*i)++] == '"');
}

void reparse_squote(t_ast_node* ret, int* i, t_token t) {
    assert(t.start[(*i)++] == '\'');

    int prev_start = *i;
    while (*i < t.len && t.start[*i] != '\'') {
        (*i)++;
    }
    vec_nd_push(&ret->children,
                create_subtoken_node(t, prev_start, *i, TT_SQWORD));
    assert(t.start[(*i)++] == '\'');
}

void reparse_norm_word(t_ast_node* ret, int* i, t_token t) {
    int prev_start = *i;
    while (*i < t.len && !is_special_char(t.start[*i])) {
        (*i)++;
    }
    printf("Pushing normal word\n");
    vec_nd_push(&ret->children,
                create_subtoken_node(t, prev_start, *i, TT_WORD));
}

t_ast_node reparse_word(t_token t) {
    t_ast_node ret = {.node_type = AST_WORD};

    for (int i = 0; i < t.len;) {
        if (t.start[i] == '"') {
            reparse_dquote(&ret, &i, t);
        } else if (t.start[i] == '\'') {
            reparse_squote(&ret, &i, t);
        } else if (t.start[i] == '$') {
            reparse_envvar(&ret, &i, t, TT_ENVVAR);
        } else if (is_space(t.start[i])) {
            assert("Unreachable" == 0);
        } else {
            reparse_norm_word(&ret, &i, t);
        }
    }
    return (ret);
}

void reparse_words(t_ast_node* node) {
    t_ast_node temp;

    if (node->node_type == AST_WORD) {
        assert(node->children.len == 1);
        temp = *node;
        *node = reparse_word(node->children.buff[0].token);
        free_ast(temp);
    }
    for (size_t i = 0; i < node->children.len; i++) {
        reparse_words(&node->children.buff[i]);
    }
}

void reparse_assignment_word(t_ast_node* word) {
    t_ast_node new_root = {.node_type = AST_ASSIGNMENT_WORD};

    if (word->children.buff[0].token.tt == TT_WORD) {
        t_token* first_token = &word->children.buff[0].token;
		if (!first_token->len || !is_var_name_p1(first_token->start[0])) {
			return;
		}
        char* eq = ft_strchr(first_token->start, '=');
        if (eq && eq <= first_token->start + first_token->len) {
			for(int i = 0; first_token->start + i < eq; i++) {
				if (!is_var_name_p2(first_token->start[i]))
					return;
			}
            vec_nd_push(&new_root.children,
                        create_subtoken_node(*first_token, 0,
                                             eq - first_token->start, TT_WORD));
            first_token->len = first_token->len - (eq - first_token->start) - 1;
            first_token->start = eq + 1;
            vec_nd_push(&new_root.children, *word);
            *word = new_root;
        }
    }
}

void reparse_assignment_words(t_ast_node* node) {
    if (node->node_type != AST_REDIRECT) {
        for (size_t i = 0; i < node->children.len; i++) {
            reparse_assignment_words(&node->children.buff[i]);
        }
    }
    if (node->node_type == AST_WORD) {
        reparse_assignment_word(node);
    }
}

t_ast_node parse_tokens(t_res* res, t_deque_tt* tokens) {
    *res = R_OK;
    t_ast_node ret = parse_simple_list(res, tokens);
    if (*res == R_FatalError) {
        printf("Parse error!\n");
    } else if (*res == R_MoreInput) {
        printf("Need more input\n");
    } else {
        t_tt tt = deque_tt_pop_start(tokens).tt;
        if (tt != TT_END) {
            printf("Got token: %s\n", tt_to_str(tt));
        }
        assert(tt == TT_END);
        assert(tokens->len == 0);
    }
    flatten(&ret);
    reparse_words(&ret);
    reparse_assignment_words(&ret);
    return (ret);
}
