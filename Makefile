TARGETS=j0 j0lex.l
ZIP_TARGETS=*.c *.h *.l Makefile
HEADERS=*.h jzero.tab.h

CC=gcc -Wall

all: ${TARGETS}

force-all:
	make clean
	make all

%.tab.c %.tab.h: %.y
	bison -d $<

%.yy.c: %.l
	flex -o $@ $<

%.o: %.c ${HEADERS}
	${CC} -c $<

j0lex.l:
	ln -s jzero.l j0lex.l

j0: main.o jzero.yy.o token.o list.o parser.o jzero.tab.o tree.o ${HEADERS}
	${CC} -o $@ $^

hw3_douglas_newquist.zip: ${ZIP_TARGETS}
	make clean
	rm -f $@
	zip $@ -r ${ZIP_TARGETS}

clean:
	rm -rf ${TARGETS} *.o *.yy.* *.tab.*
