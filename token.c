/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "jzero.tab.h"
#include "list.h"
#include "main.h"
#include "parser.h"
#include "token.h"

/*
	Creates a new token with the given category
*/
Token *create_token(int category)
{
	Token *token = (Token *)malloc(sizeof(Token));
	token->category = category;
	token->filename = current_file;
	token->line = line;
	token->column = column;

	// Copy yytext to the token
	token->text = (char *)malloc(sizeof(char) * (strlen(yytext) + 1));
	strcpy(token->text, yytext);

	// Zero values
	token->sval = NULL;
	token->cval = '\0';
	token->ival = 0;
	token->dval = 0;
	token->bval = 0;

	// Parse yytext
	switch (category)
	{
	case LITERAL_BOOL:
		token->bval = parse_bool(yytext);
		break;

	case LITERAL_INT:
		token->ival = parse_int(yytext);
		break;

	case LITERAL_DOUBLE:
		token->dval = parse_double(yytext);
		break;

	case LITERAL_CHAR:
		token->cval = parse_char(yytext);
		break;

	case LITERAL_STRING:
		token->sval = parse_string(yytext);
		break;
	}

	// Add token to list
	tokens = list_add(tokens, token);

	yylval.token = token;

	return token;
}

/*
	Frees the given token from memory
*/
void free_token(void *token)
{
	Token *t = (Token *)token;
	if (t == NULL)
		return;

	if (t->text)
		free(t->text);

	if (t->sval)
		free(t->sval);

	free(token);
}

void free_tokens()
{
	tokens = free_list(tokens, free_token);
}

void print_token_value(Token *token)
{
	switch (token->category)
	{
	case LITERAL_DOUBLE:
		printf("%f", token->dval);
		break;

	case LITERAL_INT:
		printf("%d", token->ival);
		break;

	case LITERAL_BOOL:
		printf("%d", token->bval);
		break;

	case LITERAL_STRING:
		printf("%s", token->sval);
		break;

	case LITERAL_CHAR:
		printf("%c", token->cval);
		break;
	}
}

void print_token(void *token)
{
	Token *t = (Token *)token;

	printf("%d:%d\t%d\t%s\t\t%s\t",
		   t->line,
		   t->column,
		   t->category,
		   t->text,
		   t->filename);

	print_token_value(token);

	printf("\n");
}

void print_tokens(Tokens *tokens)
{
	printf("Line\tToken\tText\t\tFile\t\t\tValue\n");
	printf("---- Tokens --------------------------------------------------\n");

	print_list(tokens, print_token);

	printf("--------------------------------------------------------------\n");
}
