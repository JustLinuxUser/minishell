#include "minishell.h"
#include <assert.h>
#include <stdio.h>

char* tt_to_str(t_tt tt) {
	switch (tt) {
		case TT_END:
			return ("TT_END");
		case TT_WORD:
			return ("TT_WORD");
		case TT_REDIRECT_LEFT:
			return ("TT_REDIRECT_LEFT");
		case TT_REDIRECT_RIGHT:
			return ("TT_REDIRECT_RIGHT");
		case TT_APPEND:
			return ("TT_APPEND");
		case TT_PIPE:
			return ("TT_PIPE");
		case TT_BRACE_LEFT:
			return ("TT_BRACE_LEFT");
		case TT_BRACE_RIGHT:
			return ("TT_BRACE_RIGHT");
		case TT_OR:
			return ("TT_OR");
		case TT_AND:
			return ("TT_AND");
		case TT_SEMICOLON:
			return ("TT_SEMICOLON");
		case TT_HEREDOC:
			return ("TT_HEREDOC");
		case TT_NEWLINE:
			return ("TT_NEWLINE");
		case TT_SQWORD:
			return ("TT_QWORD");
		case TT_DQWORD:
			return ("TT_DQWORD");
		case TT_ENVVAR:
			return ("TT_ENVVAR");
		case TT_DQENVVAR:
			return ("TT_DQENVVAR");
		case TT_NONE:
			assert(false);
			break;
	}
	return 0;
}

void print_tokens(t_deque_tt tokens) {
	t_token curr;
	printf("------- PRINTING TOKENS --------\n");
	for (int i = 0; i < tokens.len; i++) {
		curr = *deque_tt_idx(&tokens, i);
		printf("%s: >%.*s<\n", tt_to_str(curr.tt), curr.len, curr.start);
	}
	printf("------- DONE --------\n");
}
