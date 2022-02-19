/*
	Douglas Newquist
*/

#ifndef comment
#include <stdio.h>
#include "token.h"
#include "tree.h"

#if DEBUG
#define YYDEBUG 1
int yydebug;
#endif

#define TRUE 1
#define FALSE 0

#define FILE_LENGTH 256

int line, column;
char current_file[FILE_LENGTH];
FILE *yyin;
char *yytext;
Tree *program;

int yylex_destroy();

/**
 * @brief Marks the current token as a newline
 */
void newline();

/**
 * @brief Marks the current token as a comment
 */
void comment();

/**
 * @brief Marks the current toke as whitespace
 */
void whitespace();

int token(int category);

void error(int code, char *message);
void yyerror(char *message);
#endif
