# ls.c Makefile

PROG = ls
OBJS = cmp.o print.o ls.o
CFLAGS = -Wall -Werror -Wextra -g

all: ${PROG}

depend:
	mkdep -- ${CFLAGS} *.c

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} ${OBJS} -o ${PROG} ${LDFLAGS}

clean:
	rm -f ${PROG} ${OBJS}
