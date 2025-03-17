#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>
#include <stdbool.h>

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
	char prev = 0;
    while (*i < t.len && (t.start[*i] != '"' || prev == '\\')) {
        if (t.start[*i] == '$') {
            vec_nd_push(&ret->children,
                        create_subtoken_node(t, prev_start, *i, TT_DQWORD));
            reparse_envvar(ret, i, t, TT_DQENVVAR);
            prev_start = *i;
            continue;
        } else if (t.start[*i] == '"'){
            vec_nd_push(&ret->children,
                        create_subtoken_node(t, prev_start, *i, TT_DQWORD));
		}
		prev = t.start[*i];
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

void reparse_bs(t_ast_node* ret, int* i, t_token t) {
    assert(t.start[(*i)++] == '\\');

    int prev_start = *i;
	
	// bs at the end of input should have been gone by now
	assert(*i < t.len);
	(*i)++;
    vec_nd_push(&ret->children,
                create_subtoken_node(t, prev_start, *i, TT_SQWORD));
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
        } else if (t.start[i] == '\\') {
            reparse_bs(&ret, &i, t);
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
