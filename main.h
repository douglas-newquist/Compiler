/*
	Douglas Newquist
*/

#include <stdio.h>

#define TRUE 1
#define FALSE 0

int line, column;
char *current_file;
FILE *yyin;

char *yytext;
int yylex();
int yyparse();

void newline();
void comment();
void whitespace();
int token(int category);

void error(int code, char *message);
void yyerror(char *message);
