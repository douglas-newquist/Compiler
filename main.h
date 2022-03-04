/*
	Douglas Newquist
*/

#ifndef comment
#include <stdio.h>
#include "errors.h"
#include "symboltable.h"
#include "tree.h"

#if DEBUG
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
SymbolTable *table;

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

/**
 * @brief
 *
 * @param category
 */
int token(int category);
#endif
