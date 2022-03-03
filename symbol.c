#include <stdlib.h>
#include "list.h"
#include "symbol.h"

List *symbols = NULL;

Symbol *create_symbol(Token *token, char *string, int type)
{
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->string = string;
	symbol->type = type;
	symbol->token = token;
	symbol->table = NULL;

	symbols = list_add(symbols, symbol);

	return symbol;
}

void free_symbol(void *symbol)
{
	Symbol *s = (Symbol *)symbol;
	free(symbol);
}

void free_symbols()
{
	free_list(symbols, free_symbol);
	symbols = NULL;
}
