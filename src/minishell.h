/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: armgonza <armgonza@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/19 17:46:59 by anddokhn          #+#    #+#             */
/*   Updated: 2025/05/07 12:14:03 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

// debug printing of the ast
# ifndef PRINT_AST
#  define PRINT_AST 0
# else
#  define PRINT_AST 1
# endif

// debug printing of the token queue
# ifndef PRINT_TOKENS
#  define PRINT_TOKENS 0
# else
#  define PRINT_TOKENS 1
# endif

# include "dsa/vec_exe_res.h"
# include "dsa/vec_glob.h"
# include "libft/dsa/dyn_str.h"
# include "libft/dsa/vec_int.h"
# include "libft/libft.h"
# include <dirent.h>
# include <stdbool.h>
# include "dsa/vec_env.h"
# include "dsa/vec_str.h"

# define COMMAND_NOT_FOUND 127
# define EXE_PERM_DENIED 126
# define EXE_BASE_ERROR 125
# define CANCELED 130
# define SYNTAX_ERR 2
# define AMBIGUOUS_REDIRECT 1
# define PROMPT "\001❯ \002"
# define HIST_FILE ".minishell_history"

# define ANSI_RED "\001\033[31m\002"
# define ANSI_GREEN "\001\033[32m\002"
# define ANSI_RESET "\001\033[0m\002"
# define RL_SPACER_1 "\x03"

# define LEXER_SQUOTE_PROMPT "squote> "
# define LEXER_DQUOTE_PROMPT "dquote> "

extern uint32_t	g_should_unwind;

typedef enum s_res_t
{
	RES_OK,
	RES_FatalError,
	RES_MoreInput,
	RES_Init,
}	t_res_t;

typedef struct s_parser
{
	t_res_t		res;
	t_vec_int	parse_stack;
}	t_parser;

// buff_readline.c
typedef struct s_buff_readline
{
	bool		has_line;
	bool		should_update_context;
	bool		has_finished;
	int			line;
	t_dyn_str	buff;
	size_t		cursor;
}	t_buff_readline;

typedef enum e_tt
{
	TT_NONE = 0,
	TT_WORD,			// asfkaslfkj
	TT_REDIRECT_LEFT,	// <
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
}	t_tt;

typedef struct s_token_old
{
	bool	present;
	char	*start;
	int		len;
}	t_token_old;

typedef struct s_token
{
	char		*start;
	int			len;
	t_tt		tt;
	t_token_old	full_word;
	bool		allocated;
}	t_token;

typedef enum e_ast_t
{
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

typedef struct redir_s
{
	bool		direction_in;
	int			fd;
	char		*fname;
	bool		should_delete;
}	t_redir;

typedef struct s_vec_redir
{
	size_t	cap;
	size_t	len;
	t_redir	*buff;
}	t_vec_redir;

typedef struct s_vec_nd
{
	size_t				cap;
	size_t				len;
	struct s_ast_node	*buff;
}	t_vec_nd;

typedef struct s_ast_node
{
	t_ast_t		node_type;
	t_token		token;
	bool		has_redirect;
	int			redir_idx;
	t_vec_nd	children;
}	t_ast_node;

typedef struct s_deque_tt
{
	int		cap;
	int		len;
	int		start;
	int		end;
	t_token	*buff;
	char	looking_for;
}	t_deque_tt;

enum e_input_method
{
	INP_READLINE,
	INP_FILE,
	INP_ARG,
	INP_STDIN_NOTTY,
};

typedef struct s_history
{
	bool		hist_active;
	int			append_fd;
	t_vec_str	hist_cmds;
}	t_history;

typedef struct s_state
{
	t_dyn_str		input;
	t_vec_env		env;
	t_dyn_str		cwd;
	t_ast_node		tree;
	int				input_method;
	char			*base_context;
	char			*context;
	char			*pid;
	char			*last_cmd_status_s;
	t_exe_res		last_cmd_status_res;
	t_history		hist;
	bool			should_exit;
	t_vec_redir		redirects;
	int				heredoc_idx;
	t_buff_readline	readline_buff;
	t_prng_state	prng;
}	t_state;

// get next line (without \n).
// Appends the line to ret
//
// return value:
// 0 - on no input (ctrl-d)
//
// 1 - on empty line
//
// 2 - ctrl-c
//
// 3 - anything else
int			buff_readline(t_state *state, t_dyn_str *ret, char *prompt);
void		buff_readline_update(t_buff_readline *l);
void		buff_readline_reset(t_buff_readline *l);
int			get_more_input_readline(t_buff_readline *l, char *prompt);
void		update_context(t_state *state);
int			get_more_input_notty(t_state *state);

typedef struct executable_cmd_s
{
	t_vec_env	pre_assigns;
	t_vec_str	argv;
	char		*fname;
}	t_executable_cmd;

// lexer.c
typedef struct op_map_s
{
	char	*str;
	t_tt	t;
}	t_op_map;

char		*tokenizer(char *str, t_deque_tt *ret);
int			advance_dquoted(char **str);
int			advance_squoted(char **str);
void		free_all_state(t_state *state);

int			vec_redir_init(t_vec_redir *ret);
int			vec_redir_double(t_vec_redir *v);
int			vec_redir_push(t_vec_redir *v, t_redir el);
t_redir		vec_redir_pop(t_vec_redir *v);
t_redir		vec_redir_idx(t_vec_redir *v, size_t idx);

int			deque_tt_init(t_deque_tt *ret, int size);
void		deque_tt_double_if_needed(t_deque_tt *ret);
void		deque_tt_push_end(t_deque_tt *ret, t_token item);
void		deque_tt_push_start(t_deque_tt *ret, t_token item);
t_token		deque_tt_pop_end(t_deque_tt *ret);
t_token		deque_tt_pop_start(t_deque_tt *ret);
int			deque_tt_clone(t_deque_tt *ret, const t_deque_tt proto);
t_token		*deque_tt_idx(t_deque_tt *ret, int idx);

t_token		deque_tt_peek(t_deque_tt *ret);
void		deque_tt_clear(t_deque_tt *ret);

int			vec_nd_init(t_vec_nd *ret);
int			vec_nd_double(t_vec_nd *v);
int			vec_nd_push(t_vec_nd *v, t_ast_node el);
t_ast_node	vec_nd_pop(t_vec_nd *v);
t_ast_node	*vec_nd_idx(t_vec_nd *v, size_t idx);
void		vec_nd_push_vec(t_vec_nd *ret, t_vec_nd *second);
void		vec_nd_free(t_vec_nd *ret);

// parser.c
t_ast_node	parse_pipeline(t_state *state, t_parser *parser,
				t_deque_tt *tokens);
t_ast_node	parse_tokens(t_state *state, t_parser *res,
				t_deque_tt *tokens);
t_ast_node	parse_simple_list(t_state *state, t_parser *parser,
				t_deque_tt *tokens);
t_ast_node	parse_command(t_state *state, t_parser *parser, t_deque_tt *tokens);
bool		is_simple_cmd_token(t_tt tt);
t_ast_node	parse_compound_list(t_state *state, t_parser *parser,
				t_deque_tt *tokens);
t_ast_node	parse_subshell(t_state *state, t_parser *parser,
				t_deque_tt *tokens);

// tree_utils.c
void		print_node(t_ast_node node);
void		print_ast_dot(t_state *state, t_ast_node node);
void		ast_postorder_traversal(t_ast_node *node,
				void (*f)(t_ast_node *node));
void		free_ast(t_ast_node *node);

char		*tt_to_str(t_tt tt);
void		free_ast(t_ast_node *node);

void		print_tokens(t_deque_tt tokens);

// reparser.c
void		reparse_dquote(t_ast_node *ret, int *i, t_token t);
void		reparse_assignment_words(t_ast_node *node);
void		reparse_envvar(t_ast_node *ret, int *i, t_token t, t_tt tt);
void		reparse_words(t_ast_node *node);
t_ast_node	reparse_word(t_token t);
//[a-zA-Z_]
bool		is_var_name_p1(char c);
//[a-zA-Z0-9_]
bool		is_var_name_p2(char c);
t_ast_node	create_subtoken_node(t_token t,
				int offset, int end_offset, t_tt tt);

// heredoc.c
typedef struct s_heredoc_req
{
	t_dyn_str	full_file;
	bool		finished;
	char		*sep;
	bool		expand;
	bool		remove_tabs;
}	t_heredoc_req;
int			gather_heredocs(t_state *state, t_ast_node *node);
bool		contains_quotes(t_ast_node node);
void		write_heredoc(t_state *state, int wr_fd, t_heredoc_req *req);

int			env_len(char *line);
int			ft_mktemp(t_state *state, t_ast_node *node);
t_vec_env	env_to_vec_env(t_state *state, char **envp);
t_env		*env_get(t_vec_env *env, char *key);
char		*first_non_tab(char *line);
char		*env_expand(t_state *state, char *key);
char		*env_expand_n(t_state *state, char *key, int len);
void		expand_line(t_state *state, t_dyn_str *full_file, char *line);

t_env		*env_nget(t_vec_env *env, char *key, int len);
char		**get_envp(t_state *state, char *exe_path);
void		free_env(t_vec_env *env);
bool		is_special_char(char c);
bool		is_space(char c);

// expander
typedef struct s_expander_simple_cmd
{
	bool		found_first;
	bool		export;
	int			exit_stat;
	t_ast_node	*curr;
	size_t		i;
}	t_expander_simple_cmd;
t_env		assignment_to_env(t_state *state, t_ast_node *node);
void		assignment_word_to_word(t_ast_node *node);
t_token_old	get_old_token(t_ast_node word);

char		*expand_word_single(t_state *state, t_ast_node *curr);
void		expand_env_vars(t_state *state, t_ast_node *node);
void		expand_tilde_word(t_state *state, t_ast_node *curr);
// builtins
int			(*builtin_func(char *name))(t_state *state, t_vec_str argv);

// executor
typedef struct executable_node_s
{
	int			infd;
	int			outfd;
	int			next_infd;
	t_ast_node	*node;
	t_vec_int	redirs;
	bool		modify_parent_context;
}	t_executable_node;

void		execute_top_level(t_state *state);
t_exe_res	execute_pipeline(t_state *state, t_executable_node *exe);

void		set_up_redirection(t_state *state, t_executable_node *exe);
t_exe_res	execute_simple_command(t_state *state, t_executable_node *exe);
void		forward_exit_status(t_exe_res res);
t_exe_res	execute_command(t_state *state, t_executable_node *exe);
t_exe_res	execute_tree_node(t_state *state, t_executable_node *exe);
t_exe_res	execute_simple_list(t_state *state, t_executable_node *exe);
t_dyn_str	word_to_string(t_ast_node node);
t_dyn_str	word_to_hrdoc_string(t_ast_node node);
void		set_cmd_status(t_state *state, t_exe_res res);

// error.c
void		critical_error(char *error);
void		critical_error_errno(void);
void		critical_error_errno_context(char *context);
void		warning_error(char *error);
void		warning_error_errno(void);
void		err_1_errno(t_state *state, char *p1);
void		err_2(t_state *state, char *p1, char *p2);
t_ast_node	unexpected(t_state *state, t_parser *parser,
				t_ast_node ret, t_deque_tt *tokens);

// utils.c
void		free_tab(char **tab);
// 0 on success
int			write_to_file(char *str, int fd);
void		dyn_str_append_fd(int fd, t_dyn_str *ret);

// glob.c
t_vec_str	expand_word_glob(t_ast_node word);
typedef struct s_dir_matcher
{
	DIR				*dir;
	t_vec_str		*args;
	t_vec_glob		glob;
	char			*path;
	size_t			offset;
}	t_dir_matcher;
void		match_dir(t_vec_str *args,
				t_vec_glob glob, char *path, size_t offset);
size_t		matches_pattern(char *name,
				t_vec_glob patt, size_t offset, bool first);
t_vec_glob	word_to_glob(t_ast_node word);
size_t		matches_pattern(char *name, t_vec_glob patt,
				size_t offset, bool first);
void		ft_quicksort(t_vec_str *vec);

// signals.c
void		set_unwind_sig(void);
void		default_signal_handlers(void);
void		readline_bg_signals(void);
void		set_unwind_sig_norestart(void);

// expanding
void		expand_word(t_state *state, t_ast_node *node,
				t_vec_str *args, bool keep_as_one);
int			expand_simple_command(t_state *state, t_ast_node *node,
				t_executable_cmd *ret, t_vec_int *redirects);
int			redirect_from_ast_redir(t_state *state,
				t_ast_node *curr, int *redir_idx);
t_vec_nd	split_words(t_state *state, t_ast_node *node);

// hist
void		manage_history(t_state *state);
void		init_history(t_state *state);
void		free_hist(t_state *state);
void		parse_history_file(t_state *state);
t_dyn_str	encode_cmd_hist(char *cmd);

// free utils
void		free_all_state(t_state *state);
void		free_redirects(t_vec_redir *v);
void		free_executable_node(t_executable_node *node);
void		free_executable_cmd(t_executable_cmd cmd);

// prompt
t_dyn_str	prompt_normal(t_state *state);
t_dyn_str	prompt_more_input(t_parser *parser);

// init
void		init_setup(t_state *state, char **argv, char **envp);

// hack
char		*getpid_hack(void);

// execute_input
void		parse_and_execute_input(t_state *state);

// cmd_path
int			find_cmd_path(t_state *state, char *cmd_name, char **path_of_exe);

// get more tokens
void		get_more_tokens(t_state *state, char **prompt, t_deque_tt *tt);
#endif
