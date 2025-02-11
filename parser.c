#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "libft/utils/utils.h"
#include "minishell.h"

t_ast_t token_to_ast_node(t_tt tt) {
    switch (tt) {
        case TT_OR:
            return AST_OR;
        case TT_AND:
            return AST_AND;
        case TT_SEMICOLON:
            return AST_SEMICOLON;

        case TT_REDIRECT_LEFT:
        case TT_REDIRECT_RIGHT:
        case TT_APPEND:
        case TT_HEREDOC:
            return AST_REDIRECT;
        case TT_PIPE:
            return AST_COMMAND_PIPELINE;
        default:
            assert("Unreachable!" == 0);
            return (0);
            break;
    }
    return (0);
}

char* node_name(t_ast_t tn) {
    switch (tn) {
        case AST_COMMAND_PIPELINE:
            return ("AST_COMMAND_PIPELINE");
        case AST_AND:
            return ("AST_AND");
        case AST_OR:
            return ("AST_OR");
        case AST_SEMICOLON:
            return ("AST_SEMICOLON");
        case AST_REDIRECT:
            return ("AST_REDIRECT");
        case AST_ASSIGNMENT:
            return ("AST_ASSIGNMENT");
        case AST_WORD:
            return ("AST_WORD");
        case AST_TOKEN:
            return ("AST_TOKEN");
        case AST_SIMPLE_LIST:
            return ("AST_SIMPLE_LIST");
        case AST_NEWLINE:
            return ("AST_NEWLINE");
        case AST_SIMPLE_COMMAND:
            return ("AST_SIMPLE_COMMAND");
        case AST_SUBSHELL:
            return ("AST_SUBSHELL");
        case AST_REDIRECT_LIST:
            return ("AST_REDIRECT_LIST");
        case AST_COMPOUND_LIST:
            return ("AST_COMPOUND_LIST");
        case AST_COMMAND:
            return ("AST_COMMAND");
    }
    return 0;
}


bool is_word_like(t_tt tt) {
    switch (tt) {
        case TT_WORD:
		case TT_DQUOTE:
        case TT_SQUOTE:
        case TT_ENVVAR:
            return true;
        default:
            break;
    }
    return false;
}

bool is_simple_cmd_token(t_tt tt) {
	if (is_word_like(tt))
		return (true);
    switch (tt) {
        case TT_REDIRECT_LEFT:
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
        fprintf(out, " >");
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
    printf("andriish: syntax error near unexpected token %s\n", tt_to_str(deque_tt_peek(tokens).tt));
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
    t = deque_tt_pop_start(tokens);
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN, .token = t});
    return (ret);
}

t_ast_node parse_word(t_deque_tt* tokens) {
    t_ast_node ret = (t_ast_node){.node_type = AST_WORD};
    vec_nd_push(&ret.children,
                (t_ast_node){.node_type = AST_TOKEN,
                             .token = deque_tt_pop_start(tokens)});
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
        if (is_word_like(next))
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

	t_tt next =deque_tt_peek(tokens).tt;
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
    return (ret);
}
