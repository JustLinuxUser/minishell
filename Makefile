TARGET := minishell

SRCS_DIR := .
OUT_DIR := build

SOURCES := ${wildcard src/**/*.c src/*.c}

HEADERS := ${wildcard src/**/*.h src/*.h}

OBJS := ${SOURCES:%.c=${OUT_DIR}/%.o}

ifdef OPT
	CFLAGS := -fPIE -Wall -Wextra -O3 -flto
else
	CFLAGS := -Wall -Wextra -g3 -ggdb -fsanitize=address,leak
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

re: fclean all

run: all
	./${TARGET}

.PHONY: test clean fclean re
