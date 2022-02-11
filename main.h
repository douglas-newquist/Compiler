/*
	Douglas Newquist
*/

#ifndef comment
#include <stdio.h>
#include "token.h"

#define TRUE 1
#define FALSE 0

int line, column;
char *current_file;
FILE *yyin;

char *yytext;
int yylex();
int yyparse();

Token *ctoken;

void newline();
void comment();
void whitespace();
int token(int category);

void pattern(char *message);

void error(int code, char *message);
void yyerror(char *message);
#endif
