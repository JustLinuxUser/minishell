TARGET := minishell

SRCS_DIR := .
OUT_DIR := build

SOURCES := ./src/dsa/vec_glob.c\
./src/dsa/vec_env.c\
./src/dsa/vec_redir.c\
./src/dsa/vec_nd.c\
./src/dsa/deque_tt.c\
./src/dsa/vec_nd2.c\
./src/dsa/vec_str2.c\
./src/dsa/vec_str.c\
./src/dsa/vec_env4.c\
./src/dsa/vec_env2.c\
./src/dsa/deque_tt2.c\
./src/dsa/vec_env3.c\
./src/dsa/vec_exe_res.c\
./src/glob/pattern_matcher.c\
./src/glob/qsort.c\
./src/glob/glob.c\
./src/glob/glob_tokenizer.c\
./src/executor/execute_simple_list.c\
./src/executor/execute_pipeline.c\
./src/executor/executor.c\
./src/executor/execute_simple_command.c\
./src/res_utils.c\
./src/history2.c\
./src/getpwd_hack.c\
./src/history.c\
./src/expander/expander_simple_cmd.c\
./src/expander/expander_tilde.c\
./src/expander/expander_split.c\
./src/expander/expander.c\
./src/expander/expander_redirect.c\
./src/expander/expander_assignment.c\
./src/parser/parser_command.c\
./src/parser/parser_simple_list.c\
./src/parser/parser_compund_list.c\
./src/parser/parser.c\
./src/reparser/reparser2.c\
./src/reparser/reparser3.c\
./src/reparser/reparser4.c\
./src/reparser/reparser.c\
./src/heredoc/heredoc3.c\
./src/heredoc/heredoc2.c\
./src/heredoc/heredoc.c\
./src/builtins/builtins_repart.c\
./src/builtins/cd.c\
./src/builtins/echo.c\
./src/builtins/echo_flags.c\
./src/builtins/echo_help.c\
./src/builtins/exit.c\
./src/builtins/home_setter.c\
./src/builtins/pwd.c\
./src/builtins/unset.c\
./src/builtins/env.c\
./src/builtins/export.c\
./src/errors2.c\
./src/free_utils.c\
./src/main.c\
./src/tree_utils.c\
./src/errors.c\
./src/buffered_readline_readline.c\
./src/exe_path.c\
./src/utils.c\
./src/buffered_readline_utils.c\
./src/lexer.c\
./src/init.c\
./src/print_utils.c\
./src/debug_tokens.c\
./src/signals.c\
./src/buffered_readline.c\
./src/get_more_tokens.c\
./src/prompt.c\
./src/lexer2.c\
./src/process_input.c

HEADERS := ./src/dsa/vec_glob.h\
./src/dsa/vec_str.h\
./src/dsa/vec_exe_res.h\
./src/dsa/vec_env.h\
./src/builtins/builtins.h\
./src/minishell.h

OBJS := ${SOURCES:%.c=${OUT_DIR}/%.o}

ifdef OPT
	CFLAGS := -fPIE -Wall -Wextra -O3 -flto ${FLAGS}
else
	CFLAGS := -Wall -Wextra -g3 -ggdb -fsanitize=address,leak ${FLAGS}
endif

LIBFT_A := src/libft/libft.a

LIBS := -L src/libft -lft -lreadline

CC := cc

all: ${TARGET} ${headers}

src/libft/libft.a: src/libft
	cd src/libft && make

${TARGET}: Makefile build ${OBJS} ${LIBFT_A}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJS} ${LIBS}

${OUT_DIR}/%.o: ${SRCS_DIR}/%.c ${HEADERS}
	mkdir -p $$(dirname $@)
	${CC} ${CFLAGS} -c $< -o $@

${OUT_DIR}:
	mkdir -p build

clean:
	rm -rf ${OUT_DIR}
	make -C src/libft fclean

fclean: clean
	rm -rf ${TARGET}

re:: fclean
re:: all

run: all
	./${TARGET}

.PHONY: test clean fclean re
