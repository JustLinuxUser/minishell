#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"

char* node_name(t_ast_t tn) {
    switch (tn) {
        case AST_COMMAND_PIPELINE:
            return ("AST_COMMAND_PIPELINE");
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

void print_node(t_ast_node node) {
    ft_printf(" (%s", node_name(node.node_type));
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

void ast_postorder_traversal(t_ast_node* node, void (*f)(t_ast_node* node)) {
    size_t i;

    i = 0;
    while (i < node->children.len) {
        ast_postorder_traversal(vec_nd_idx(&node->children, i), f);
        i++;
    }
    f(node);
}
