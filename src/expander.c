#include "dsa/vec_str.h"
#include "libft/ft_printf/ft_printf.h"
#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>

t_dyn_str	word_to_string(t_ast_node node)
{
	t_token		curr;
	t_dyn_str	s;
	size_t		i;

	dyn_str_init(&s);
	i = 0;
	while (i < node.children.len)
	{
		assert(node.children.buff[i].node_type == AST_TOKEN);
		curr = node.children.buff[i].token;
		if (curr.tt == TT_WORD || curr.tt == TT_SQWORD
			|| curr.tt == TT_DQWORD || curr.tt == TT_DQENVVAR
			|| curr.tt == TT_ENVVAR)
		{
			assert(curr.start || !curr.len);
			dyn_str_pushnstr(&s, curr.start, curr.len);
		}
		else
		{
			printf("got unexpected: %s\n", tt_to_str(curr.tt));
			assert("Unreachable" == 0);
		}
		i++;
	}
	return (s);
}

t_dyn_str	word_to_hrdoc_string(t_ast_node node)
{
	t_token		curr;
	t_dyn_str	s;
	size_t		i;

	dyn_str_init(&s);
	i = 0;
	while (i < node.children.len)
	{
		assert(node.children.buff[i].node_type == AST_TOKEN);
		curr = node.children.buff[i].token;
		if (curr.tt == TT_WORD || curr.tt == TT_SQWORD
			|| curr.tt == TT_DQWORD)
			dyn_str_pushnstr(&s, curr.start, curr.len);
		else if (curr.tt == TT_DQENVVAR || curr.tt == TT_ENVVAR)
		{
			dyn_str_push(&s, '$');
			dyn_str_pushnstr(&s, curr.start, curr.len);
		}
		else
			assert("Unreachable" == 0);
		i++;
	}
	return (s);
}

t_env	assignment_to_env(t_state *state, t_ast_node *node)
{
	t_vec_str	args;
	t_env		ret;

	ret = (t_env){};
	vec_str_init(&args);
	assert(node->children.len == 2);
	expand_word(state, &node->children.buff[1], &args, true);
	assert(node->children.buff[1].children.buff == 0);
	ret.key = ft_strndup(node->children.buff[0].token.start,
			node->children.buff[0].token.len);
	if (args.len)
	{
		assert(args.len == 1);
		ret.value = args.buff[0];
		if (!ret.value)
			ret.value = ft_strdup("");
	}
	free(args.buff);
	return (ret);
}

void	assignment_word_to_word(t_ast_node *node)
{
	t_ast_node	ret;
	t_ast_node	left;
	t_ast_node	right;

	ret = (t_ast_node){.node_type = AST_WORD};
	assert(node->node_type == AST_ASSIGNMENT_WORD);
	assert(node->children.len == 2);
	left = node->children.buff[0];
	right = node->children.buff[1];
	left.token.len++;
	vec_nd_push(&ret.children, left);
	vec_nd_push_vec(&ret.children, &right.children);
	free(right.children.buff);
	free(node->children.buff);
	*node = ret;
}

char	*expand_word_single(t_state *state, t_ast_node *curr)
{
	t_vec_str	args;
	size_t		i;
	char		*temp;

	vec_str_init(&args);
	expand_word(state, curr, &args, false);
	if (args.len != 1)
	{
		i = 0;
		while (i < args.len)
		{
			free(args.buff[i]);
			i++;
		}
		free(args.buff);
		return (0);
	}
	temp = args.buff[0];
	free(args.buff);
	return (temp);
}

t_token_old	get_full_word_from_word(t_ast_node word)
{
	t_token_old	ret;

	assert(word.node_type == AST_WORD);
	assert(word.children.len);
	ret = word.children.buff[0].token.full_word;
	assert(ret.present);
	return (ret);
}

bool	create_redir(t_tt tt, char *fname, t_redir *ret)
{
	assert(tt != TT_HEREDOC && "HEREDOCS are handled separately");
	ret->fname = fname;
	ret->direction_in = tt == TT_REDIRECT_LEFT;
	if (!ret->fname)
		return (false);
	if (tt == TT_REDIRECT_LEFT)
		ret->fd = open(ret->fname, O_RDONLY);
	else if (tt == TT_REDIRECT_RIGHT)
		ret->fd = open(ret->fname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	else if (tt == TT_APPEND)
		ret->fd = open(ret->fname, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (ret->fd < 0)
	{
		free(ret->fname);
		return (false);
	}
	ret->should_delete = false;
	return (true);
}

int	redirect_from_ast_redir(t_state *state, t_ast_node *curr, int *redir_idx)
{
	t_redir		ret;
	t_tt		tt;
	t_token_old	full_token;
	char		*fname;

	assert(curr->node_type == AST_REDIRECT);
	if (curr->has_redirect)
	{
		ret = state->redirects.buff[curr->redir_idx];
		return (0);
	}
	tt = curr->children.buff[0].token.tt;
	full_token = get_full_word_from_word(curr->children.buff[1]);
	fname = expand_word_single(state, vec_nd_idx(&curr->children, 1));
	if (!create_redir(tt, fname, &ret))
	{
		ft_eprintf("%s: %.*s: ambigous redirect\n",
			state->context, full_token.len, full_token.start);
		return (-1);
	}
	curr->redir_idx = state->redirects.len;
	*redir_idx = state->redirects.len;
	curr->has_redirect = true;
	vec_redir_push(&state->redirects, ret);
	return (0);
}

void	expand_node_glob(t_ast_node *node, t_vec_str *args, bool keep_as_one)
{
	t_vec_str	glob_words;
	t_dyn_str	temp;
	size_t		j;

	glob_words = expand_word_glob(*node);
	if (g_should_unwind)
		return ;
	dyn_str_init(&temp);
	j = 0;
	while (j < glob_words.len)
	{
		if (!keep_as_one)
			vec_str_push(args, glob_words.buff[j]);
		else
		{
			dyn_str_pushstr(&temp, glob_words.buff[j]);
			free(glob_words.buff[j]);
		}
		if (j++ + 1 < glob_words.len && keep_as_one)
			dyn_str_push(&temp, ' ');
	}
	if (keep_as_one)
		vec_str_push(args, temp.buff);
	free(glob_words.buff);
	free_ast(node);
}

void	expand_word(t_state *state, t_ast_node *node,
	t_vec_str *args, bool keep_as_one)
{
	t_vec_nd	words;
	size_t		i;

	expand_tilde_word(state, node);
	expand_env_vars(state, node);
	vec_nd_init(&words);
	if (!keep_as_one)
		words = split_words(node);
	else
	{
		vec_nd_push(&words, *node);
		*node = (t_ast_node){};
	}
	i = 0;
	while (i < words.len)
	{
		expand_node_glob(&words.buff[i], args, keep_as_one);
		if (g_should_unwind)
			while (i < words.len)
				free_ast(&words.buff[i++]);
		if (g_should_unwind)
			break ;
		i++;
	}
	return (free(words.buff), free_ast(node));
}

bool	is_export(t_ast_node word)
{
	t_ast_node	c;

	if (word.children.len != 1)
		return (false);
	c = word.children.buff[0];
	if (c.token.tt != TT_WORD)
		return (false);
	if (ft_strncmp(c.token.start, "export", c.token.len))
		return (false);
	return (true);
}

int	expand_simple_command(t_state *state, t_ast_node *node, t_executable_cmd *ret, t_vec_int *redirects)
{
	bool		found_first;
	bool		export;
	t_ast_node	*curr;
	size_t		i;
	int			redir_idx;

	*ret = (t_executable_cmd){};
	found_first = false;
	export = false;
	vec_str_init(&ret->argv);
	i = 0;
	while (i < node->children.len)
	{
		curr = &node->children.buff[i];
		if (curr->node_type == AST_WORD)
		{
			if (!found_first && is_export(*curr))
				export = true;
			expand_word(state, curr, &ret->argv, false);
			if (g_should_unwind)
				return (1);
			found_first = true;
		}
		else if (curr->node_type == AST_ASSIGNMENT_WORD)
		{
			if (!found_first)
				vec_env_push(&ret->pre_assigns, assignment_to_env(state, curr));
			else
			{
				assignment_word_to_word(curr);
				print_node(*curr);
				if (export)
					expand_word(state, curr, &ret->argv, true);
				else
					expand_word(state, curr, &ret->argv, false);
				if (g_should_unwind)
					return (1);
			}
		}
		else if (curr->node_type == AST_REDIRECT)
		{
			if (redirect_from_ast_redir(state, curr, &redir_idx))
				return (AMBIGUOUS_REDIRECT);
			vec_int_push(redirects, redir_idx);
		}
		else
			assert("Unimplemented" == 0);
		i++;
	}
	return (0);
}
