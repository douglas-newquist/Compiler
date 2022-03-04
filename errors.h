/*
	Douglas Newquist
*/

#include "token.h"

#ifndef LEX_ERROR
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEMATIC_ERROR 3

void error(int code, char *message);
void yyerror(char *message);

/**
 * @brief Calls error at the given token's position
 */
void error_at(Token *token, int code, char *message);
#endif
