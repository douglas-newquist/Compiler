/*
	Douglas Newquist
*/

#ifndef comment
#include <stdio.h>
#include "token.h"
#include "tree.h"

#define TRUE 1
#define FALSE 0

int line, column;
Token *ctoken;
Tokens *tokens;
char *current_file;
FILE *yyin;

Node *program;

char *yytext;
int yylex();
int yyparse();

void newline();
void comment();
void whitespace();
int token(int category);

void pattern(char *message);

void error(int code, char *message);
void yyerror(char *message);
#endif
