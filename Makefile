TARGETS=j0
ZIP_TARGETS=*.c *.h *.l *.y Makefile
HEADERS=*.h jzero.tab.h

SOURCES=$(shell find -name "*.c" -not -name "*.*.c") jzero.tab.c jzero.yy.c
OBJECTS=$(SOURCES:%.c=%.o)

CC=gcc -Wall

all: ${TARGETS}

force-all:
	make clean
	make all

debug: jzero.output

%.tab.c %.tab.h: %.y
	bison -d $<

%.output: %.y
	bison -v -d $<

%.yy.c: %.l
	flex -o $@ $<

%.o: %.c ${HEADERS}
	${CC} -c $<

j0: ${OBJECTS} ${HEADERS}
	${CC} -o $@ ${OBJECTS}

hw3_douglas_newquist.zip: ${ZIP_TARGETS}
	make clean
	rm -f $@
	zip $@ -r ${ZIP_TARGETS}

clean:
	rm -rf ${TARGETS} *.o *.yy.* *.tab.*
