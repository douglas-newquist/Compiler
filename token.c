/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "tokens.h"
#include "main.h"
#include "parser.h"

/*
	Creates a new token with the given category
*/
struct token *create_token(int category)
{
	struct token *token = (struct token *)malloc(sizeof(struct token));
	token->category = category;
	token->filename = current_file;
	token->line = line;

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

	return token;
}

/*
	Frees the given token from memory
*/
void free_token(struct token *token)
{
	free(token->text);

	if (token->sval)
		free(token->sval);

	free(token);
}

void free_tokens(struct tokenlist *tokens)
{
	while (tokens != NULL)
	{
		struct tokenlist *next = tokens->next;
		free_token(tokens->token);
		free(tokens);
		tokens = next;
	}
}

void print_token(struct token *token)
{
	printf("%d\t%d\t%s\t\t%s\t",
		   token->line,
		   token->category,
		   token->text,
		   token->filename);

	switch (token->category)
	{
	case LITERAL_DOUBLE:
		printf("%f\n", token->dval);
		break;

	case LITERAL_INT:
		printf("%d\n", token->ival);
		break;

	case LITERAL_BOOL:
		printf("%d\n", token->bval);
		break;

	case LITERAL_STRING:
		printf("%s\n", token->sval);
		break;

	case LITERAL_CHAR:
		printf("%c\n", token->cval);
		break;

	default:
		printf("\n");
	}
}

void print_tokens(struct tokenlist *tokens)
{
	printf("Line\tToken\tText\t\tFile\t\t\tValue\n");
	printf("--------------------------------------------------------------\n");

	while (tokens != NULL)
	{
		print_token(tokens->token);
		tokens = tokens->next;
	}

	printf("--------------------------------------------------------------\n");
}

struct tokenlist *create_tokens(struct token *token)
{
	struct tokenlist *tokens = (struct tokenlist *)malloc(sizeof(struct tokenlist));
	tokens->token = token;
	tokens->next = NULL;
	return tokens;
}

/*
	Gets the tail element of the token list\
*/
struct tokenlist *tail(struct tokenlist *tokens)
{
	if (tokens == NULL)
		return NULL;

	while (tokens->next != NULL)
		tokens = tokens->next;

	return tokens;
}

/*
	Adds a token to the linked list
*/
struct tokenlist *add(struct tokenlist *tokens, struct token *token)
{
	struct tokenlist *current = create_tokens(token);

	if (tokens != NULL)
	{
		tail(tokens)->next = current;
		return tokens;
	}

	return current;
}
