TARGETS=j0 j0lex.l
ZIP_TARGETS=*.c *.h *.l Makefile

all: ${TARGETS}

%.yy.c: %.l *.h
	flex -o $@ $<

%.o: %.c *.h
	gcc -Wall -c $<

j0lex.l:
	ln -s jzero.l j0lex.l

j0: main.o jzero.yy.o token.o parser.o
	gcc -Wall -o $@ $^

hw2_douglas_newquist.zip: ${ZIP_TARGETS}
	make clean
	rm -f $@
	zip $@ -r ${ZIP_TARGETS}

clean:
	rm -rf ${TARGETS} *.o *.yy.c
