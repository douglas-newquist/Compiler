/*
	Douglas Newquist
*/

#include <stdio.h>

#define TRUE 1
#define FALSE 0

int line;
char *current_file;
FILE *yyin;

char *yytext;
int yylex();

void newline();
void comment();
void whitespace();
void yyerror(int code, char *message);
void yyerror(char *message);
