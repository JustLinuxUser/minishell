#ifndef MINISHELL_H
# define MINISHELL_H

# include "dsa/vec_exe_res.h"
# include "libft/dsa/dyn_str.h"
# include <stdbool.h>
# include "dsa/vec_env.h"
# include "dsa/vec_str.h"

# define COMMAND_NOT_FOUND 127
# define EXE_PERM_DENIED 126

typedef enum s_res {
    R_OK,
    R_FatalError,
    R_MoreInput,
} t_res;

typedef enum e_tt {
	TT_NONE = 0,
    TT_WORD,			// asfkaslfkj
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

	TT_SQWORD,
	TT_DQWORD,

	TT_ENVVAR,
	TT_DQENVVAR,
} t_tt;

typedef struct s_token {
    char* start;
    int len;
    t_tt tt;
	bool allocated;
} t_token;


typedef enum e_ast_t {
	AST_SIMPLE_LIST,
	AST_COMMAND_PIPELINE,

	AST_REDIRECT,
	AST_SIMPLE_COMMAND,
	AST_SUBSHELL,
	AST_COMPOUND_LIST,

	AST_COMMAND,
	AST_WORD,
	AST_ASSIGNMENT_WORD,
	AST_TOKEN,

}	t_ast_t;

typedef struct redir_s {
	bool	direction_in;
	int		fd;
	char	*fname;
	bool	should_delete;
} redir_t;

typedef struct s_vec_redir
{
	size_t	cap;
	size_t	len;
	redir_t		*buff;
}	t_vec_redir;

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
	redir_t *redir;

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

typedef struct state_s {
	t_dyn_str	prompt;
	int			parse_idx;
	t_vec_env	env;
	t_ast_node	tree;
	char		**argv;
	int			last_cmd_status;
} t_state;

typedef struct executable_cmd_s {
	t_vec_env pre_assigns;
	t_vec_str argv;
	char *fname;
} executable_cmd_t;

// lexer.c
char* tokenizer(char* str, t_deque_tt* ret);

int	vec_redir_init(t_vec_redir *ret);
int	vec_redir_double(t_vec_redir *v);
int	vec_redir_push(t_vec_redir *v, redir_t el);
redir_t	vec_redir_pop(t_vec_redir *v);
redir_t	vec_redir_idx(t_vec_redir *v, size_t idx);

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

// tree_utils.c
void		print_node(t_ast_node node);
void		print_ast_dot(t_ast_node node);
void ast_postorder_traversal(t_ast_node* node, void (*f)(t_ast_node* node));

char*		tt_to_str(t_tt tt);
void		free_ast(t_ast_node node);

// reparser.c
void reparse_assignment_words(t_ast_node* node);
void reparse_words(t_ast_node* node);

t_ast_node create_subtoken_node(t_token t, int offset, int end_offset, t_tt tt);


t_vec_env env_to_vec_env(char** envp);
t_env* env_get(t_vec_env* env, char* key);
t_env* env_nget(t_vec_env* env, char* key, int len);
void free_env(t_vec_env *env);
bool is_special_char(char c);
bool is_space(char c);

void execute_top_level(t_state *state);


// builtins
int (*builtin_func(char *name)) (t_state *state, t_vec_str argv);

// executor
typedef struct executable_node_s {
	int			infd;
	int			outfd;
	t_ast_node	*node;
	t_vec_redir redirs;
	bool		modify_parent_context;
} t_executable_node;

t_exe_res execute_command(t_state* state, t_executable_node exe);
#endif
