#ifndef MINISHELL_H
# define MINISHELL_H

#include "libft/dsa/dyn_str.h"
#include <stdbool.h>
#include "vec_env.h"

typedef enum s_res {
    R_OK,
    R_FatalError,
    R_MoreInput,
} t_res;

typedef enum e_tt {
	TT_NONE = 0,
    TT_WORD,			// asfkaslfkj
	TT_BSWORD,			// \n
	TT_DQUOTE,			// "fafd"
	TT_SQUOTE,			// 'fadf'
	TT_ENVVAR,			// $FFKFF
	TT_SEP,				//    	
    TT_REDIRECT_LEFT, 	// <
    TT_REDIRECT_RIGHT,	// >
    TT_APPEND,			// >>
	TT_PIPE,			// |
	TT_BRACE_LEFT,		// (
	TT_BRACE_RIGHT,		// )
	TT_OR,				// ||
	TT_AND,				// &&
	TT_SEMICOLON,		// ;
	TT_HEREDOC,			// << | <<-
    TT_NEWLINE,			// '\n'
    TT_END,
} t_tt;

typedef struct s_token {
    char* start;
    int len;
    t_tt tt;
	int quoted;
} t_token;

typedef struct state_s {
	t_dyn_str	prompt;
	int			parse_idx;
	t_vec_env	env;
} t_state;

typedef enum e_ast_t {
	AST_SIMPLE_LIST,
	AST_COMMAND_PIPELINE,

	AST_AND,
	AST_OR,
	AST_SEMICOLON,
	AST_NEWLINE,
	AST_REDIRECT,
	AST_SIMPLE_COMMAND,
	AST_SUBSHELL,
	AST_REDIRECT_LIST,
	AST_COMPOUND_LIST,

	AST_ASSIGNMENT,
	AST_COMMAND,
	AST_WORD,
	AST_TOKEN,
}	t_ast_t;

typedef struct s_ast_node t_ast_node;

typedef struct s_vec_nd
{
	size_t		cap;
	size_t		len;
	t_ast_node	*buff;
}	t_vec_nd;

typedef struct s_ast_node {
    t_ast_t node_type;
    t_token token;
	t_vec_nd children;
} t_ast_node;

typedef struct s_deque_tt
{
	int		cap;
	int		len;
	int		start;
	int		end;
	t_token	*buff;
}	t_deque_tt;

int		deque_tt_init(t_deque_tt *ret, int size);
void	deque_tt_double_if_needed(t_deque_tt *ret);
void	deque_tt_push_end(t_deque_tt *ret, t_token item);
void	deque_tt_push_start(t_deque_tt *ret, t_token item);
t_token	deque_tt_pop_end(t_deque_tt *ret);
t_token	deque_tt_pop_start(t_deque_tt *ret);
int		deque_tt_clone(t_deque_tt *ret, const t_deque_tt proto);
t_token	*deque_tt_idx(t_deque_tt *ret, int idx);
t_token	*deque_tt_idx_wrapping(t_deque_tt *ret, int idx);

t_token deque_tt_peek(t_deque_tt *ret);
t_token deque_tt_peek2(t_deque_tt *ret);
bool	deque_tt_check2(t_deque_tt *ret, t_tt t1, t_tt t2);
bool	deque_tt_check1(t_deque_tt *ret, t_tt t1);

int			vec_nd_init(t_vec_nd *ret);
int			vec_nd_double(t_vec_nd *v);
int			vec_nd_push(t_vec_nd *v, t_ast_node el);
t_ast_node	vec_nd_pop(t_vec_nd *v);
t_ast_node	*vec_nd_idx(t_vec_nd *v, size_t idx);
void		vec_nd_push_vec(t_vec_nd *ret, t_vec_nd *second);
void		vec_nd_free(t_vec_nd *ret);
t_ast_node parse_tokens(t_res *res, t_deque_tt* tokens);
void		print_node(t_ast_node node);
void		print_ast_dot(t_ast_node node);
char*		tt_to_str(t_tt tt);
void		free_ast(t_ast_node node);

t_vec_env env_to_vec_env(char** envp);
t_env* env_get(t_vec_env* env, char* key);
void free_env(t_vec_env *env);
#endif
