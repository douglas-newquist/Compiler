/*
	Douglas Newquist
*/

#ifndef LEX_ERROR

#include "token.h"

#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEMATIC_ERROR 3
#define OUT_OF_MEMORY -1

void error(int code, char *message);
void yyerror(char *message);

char *error_message(char *pattern, ...);

/**
 * @brief Calls error at the given token's position
 */
void error_at(Token *token, int code, char *message);
#endif
