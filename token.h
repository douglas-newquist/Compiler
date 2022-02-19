/*
	Douglas Newquist
*/

#include "list.h"

#ifndef Token
#define Token struct token
#define Tokens List

Tokens *tokens;

Token
{
	int category;
	char *text;
	int line, column;
	char *filename;

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
 *	@brief Frees the given token from memory
 */
void free_token(Token *token);

/*
	Frees the given token list from memory
*/
void free_tokens();

/**
 * @brief Prints the parsed version of this token's text
 */
void print_token_value(Token *token);
void print_token(Token *token);
void print_tokens(Tokens *tokens);

#endif
