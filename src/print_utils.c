/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anddokhn <anddokhn@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/18 21:24:06 by anddokhn          #+#    #+#             */
/*   Updated: 2025/04/18 21:24:23 by anddokhn         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft/ft_printf/ft_printf.h"
#include "minishell.h"
#include <assert.h>
#include <stdio.h>

char	*node_name(t_ast_t tn)
{
	if (tn == AST_COMMAND_PIPELINE)
		return ("AST_COMMAND_PIPELINE");
	if (tn == AST_REDIRECT)
		return ("AST_REDIRECT");
	if (tn == AST_WORD)
		return ("AST_WORD");
	if (tn == AST_TOKEN)
		return ("AST_TOKEN");
	if (tn == AST_SIMPLE_LIST)
		return ("AST_SIMPLE_LIST");
	if (tn == AST_SIMPLE_COMMAND)
		return ("AST_SIMPLE_COMMAND");
	if (tn == AST_SUBSHELL)
		return ("AST_SUBSHELL");
	if (tn == AST_COMPOUND_LIST)
		return ("AST_COMPOUND_LIST");
	if (tn == AST_COMMAND)
		return ("AST_COMMAND");
	if (tn == AST_ASSIGNMENT_WORD)
		return ("AST_ASSIGNMENT_WORD");
	assert("Unreachable" == 0);
}

void	print_node(t_ast_node node)
{
	size_t	i;

	ft_printf(" (%s", node_name(node.node_type));
	if (node.node_type == AST_TOKEN)
	{
		printf(" >%.*s<", node.token.len, node.token.start);
		fflush(stdout);
	}
	i = 0;
	while (i < node.children.len)
		print_node(*vec_nd_idx(&node.children, i++));
	ft_printf(")");
}

void	print_token_str(t_ast_node node, FILE *out)
{
	int		i;
	char	c;

	i = 0;
	while (i < node.token.len)
	{
		c = node.token.start[i];
		if (c == '\\')
			fprintf(out, "\\\\");
		else if (c == '"')
			fprintf(out, "\\\"");
		else if (c == '\'')
			fprintf(out, "\\'");
		else
			fprintf(out, "%c", c);
		i++;
	}
}

void	print_dot_node(t_ast_node node, int id, FILE *out)
{
	size_t	i;
	int		r;

	fprintf(out, "	n%i [label=\"%s", id, node_name(node.node_type));
	if (node.node_type == AST_TOKEN)
	{
		fprintf(out, " %s: >", tt_to_str(node.token.tt));
		print_token_str(node, out);
		fprintf(out, "<");
	}
	fprintf(out, "\"];\n");
	i = 0;
	while (i < node.children.len)
	{
		r = rand();
		fprintf(out, "	n%i -> n%i;\n", id, r);
		print_dot_node(*vec_nd_idx(&node.children, i), r, out);
		i++;
	}
}

void	print_ast_dot(t_ast_node node)
{
	FILE	*out;

	out = fopen("out", "w");
	fprintf(out, "digraph G {\n");
	print_dot_node(node, 0, out);
	fprintf(out, "}\n");
	fclose(out);
}
