TARGETS=j0 jzero.tab.h
ZIP_TARGETS=*.c *.h *.l *.y README Makefile tests
HEADERS=*.h jzero.tab.h

SOURCES=$(shell find -name "*.c" -not -name "*.*.c") jzero.tab.c jzero.yy.c
OBJECTS=$(SOURCES:%.c=%.o)

CC=gcc -Wall

all: ${TARGETS}

force-all:
	make clean
	make all

force-debug:
	make clean
	make debug

debug: DEBUG=-DDEBUG
debug: jzero.output ${TARGETS}

%.tab.c %.tab.h: %.y
	bison -d $<

%.output: %.y
	bison -v -d $<

%.yy.c: %.l
	flex -o $@ $<

%.o: %.c ${HEADERS}
	${CC} ${DEBUG} -c $<

j0: ${OBJECTS} ${HEADERS}
	${CC} ${DEBUG} -o $@ ${OBJECTS}

%.zip: ${ZIP_TARGETS}
	make clean
	rm -f $@
	zip $@ -r ${ZIP_TARGETS}

clean:
	rm -rf ${TARGETS} *.o *.yy.* *.tab.* *.output
