TARGET := minishell

SRCS_DIR := .
OUT_DIR := build

SOURCES := ${wildcard *.c}
# deque_tt2.c\
# deque_tt.c\
# executor.c\
# main.c\
# parser.c\
# prompt.c\
# vec_env2.c\
# vec_env.c\
# vec_nd2.c\
# vec_nd.c

HEADERS := ${wildcard *.h}

OBJS := ${SOURCES:%.c=${OUT_DIR}/%.o}

CFLAGS := -Wall -Wextra -g3 -ggdb -fsanitize=address -fno-inline-small-functions

LIBS := -L libft -lft -lreadline

CC := gcc

all: ${TARGET} ${headers}

libft/libft.a: libft
	cd libft && make

${TARGET}: Makefile build ${OBJS} libft/libft.a
	${CC} ${CFLAGS} -o ${TARGET} ${OBJS} ${LIBS}

${OUT_DIR}/%.o: ${SRCS_DIR}/%.c ${HEADERES} 
	${CC} ${CFLAGS} -c $< -o $@

${OUT_DIR}:
	mkdir -p build

clean:
	rm -rf ${OUT_DIR}
	make -C libft fclean

fclean: clean
	rm -rf ${TARGET}

re: fclean all

run: all
	./${TARGET}

.PHONY: test clean fclean re
