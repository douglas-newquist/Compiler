/*
	Douglas Newquist
*/

#ifndef Token

#define Token struct token

#include "list.h"
#include "symbol.h"

List *tokens;

Token
{
	int id;
	int category;
	char *text;
	int line, column;
	char *filename;

	Symbol *symbol;

	// Integer value
	int ival;
	// Double value
	double dval;
	// String value
	char *sval;
	// Boolean value
	short bval;
	// Character value
	char cval;
};

/**
 * @brief Creates a new token with the given category
 *
 * @param category What category this token falls in
 */
Token *create_token(int category);

/**
 * @brief Prints the parsed version of this token's text
 */
void print_token_value(Token *token);
void print_token(void *token);
void print_tokens(List *tokens);

#endif
