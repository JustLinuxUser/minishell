/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 14:56:57 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/11 16:54:32 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/libft.h"
#include "minishell.h"
#include <assert.h>

bool	is_space(char c)
{
	if (c == ' ' || c == '\t')
		return (true);
	return (false);
}

bool	is_special_char(char c)
{
	char	*specials;

	specials = ";$'\"<>|&()\n";
	if (ft_strchr(specials, c) || is_space(c))
		return (true);
	return (false);
}

int	advance_dquoted(char **str)
{
	char	prev;

	assert(**str == '\"');
	(*str)++;
	prev = 0;
	while (**str && (**str != '\"' || prev == '\\'))
	{
		prev = **str;
		(*str)++;
	}
	if (**str != '\"')
		return (1);
	(*str)++;
	return (0);
}

int	advance_squoted(char **str)
{
	char	prev;

	assert(**str == '\'');
	(*str)++;
	prev = 0;
	while (**str && (**str != '\'' || prev == '\\'))
	{
		prev = **str;
		(*str)++;
	}
	if (**str != '\'')
		return (1);
	(*str)++;
	return (0);
}

static char	*parse_word(t_deque_tt *tokens, char **str)
{
	char	*start;

	start = *str;
	while (1)
	{
		if (**str == '\\')
			(*str) += 2;
		else if (!is_special_char(**str) || **str == '$')
			(*str)++;
		else if (**str == '\'')
		{
			if (advance_squoted(str))
				return ("squote> ");
		}
		else if (**str == '"')
		{
			if (advance_dquoted(str))
				return ("dquote> ");
		}
		else
			break ;
	}
	deque_tt_push_end(
		tokens, (t_token){.start = start, .len = *str - start, .tt = TT_WORD});
	return (0);
}

typedef struct op_map_s {
	char* str;
	t_tt t;
} op_map_t;

int	longest_matching_str(op_map_t *needles, char *haystack)
{
	int	max_idx;
	int	max;
	int	i;

	max_idx = -1;
	max = -1;
	i = 0;
	while (needles[i].str)
	{
		if ((int)ft_strlen(needles[i].str) > max
			&& ft_strncmp(needles[i].str, haystack,
				ft_strlen(needles[i].str)) == 0)
		{
			max_idx = i;
			max = ft_strlen(needles[i].str);
		}
		i++;
	}
	return (max_idx);
}

void	parse_op(t_deque_tt *tokens, char **str)
{
	op_map_t operators[] = {
		(op_map_t){"|", TT_PIPE},
		(op_map_t){"<<", TT_HEREDOC},
		(op_map_t){"<<-", TT_HEREDOC},
		(op_map_t){">>", TT_APPEND},
		(op_map_t){"(", TT_BRACE_LEFT},
		(op_map_t){")", TT_BRACE_RIGHT},
		(op_map_t){">", TT_REDIRECT_RIGHT},
		(op_map_t){"<", TT_REDIRECT_LEFT},
		(op_map_t){"&&", TT_AND},
		(op_map_t){"&", TT_END},
		(op_map_t){"||", TT_OR},
		(op_map_t){";", TT_SEMICOLON},
		(op_map_t){0, TT_END},
	};

	char* start = *str;
	int op_idx = longest_matching_str(operators, *str);
	assert(op_idx != -1);
	*str += ft_strlen(operators[op_idx].str);
	deque_tt_push_end(tokens, (t_token){.start = start,
		.len = *str - start,
		.tt = operators[op_idx].t});
}

// If returns 0, it finished properly, if it returns a ptr, make a prompt
// with that str
char* tokenizer(char* str, t_deque_tt* ret) {
	char* prompt = 0;

	while (*str) {
		if (*str == '\'' || *str == '"' || *str == '$' ||
			!(is_special_char(*str))) {
			prompt = parse_word(ret, &str);
		} else if (*str == '\n') {
			deque_tt_push_end(
				ret, (t_token){.start = str, .len = 1, .tt = TT_NEWLINE});
			str++;
		} else if (is_space(*str)) {
			str++;
		} else {
			parse_op(ret, &str);
		}
		if (prompt)
			break;
	}
	deque_tt_push_end(ret, (t_token){.tt = TT_END});
	return (prompt);
}
