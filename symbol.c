#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "symbol.h"
#include "symboltable.h"

int symbol_count = 0;
List *symbols = NULL;

Symbol *create_symbol(Token *token, char *string, int type)
{
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->id = symbol_count++;
	symbol->string = string;
	symbol->type = type;
	symbol->attributes = 0;
	symbol->token = token;
	symbol->table = NULL;

	symbols = list_add(symbols, symbol);

	return symbol;
}

void free_symbol(void *symbol)
{
	free(symbol);
}

void free_symbols()
{
	free_list(symbols, free_symbol);
	symbols = NULL;
}

void print_symbol(void *symbol)
{
	Symbol *s = (Symbol *)symbol;

	printf("%d %s of type %d ", s->id, s->string, s->type);

	if (s->table == NULL)
		printf("<Orphan>\n");
	else
		printf("in table %d", s->table->id);

	if (s->table->parent == NULL)
		printf("\n");
	else
		printf(" in %d\n", s->table->parent->id);
}

void print_symbols()
{
	printf("---- Symbols -------------------------------------------------\n");
	print_list(symbols, print_symbol);
	printf("--------------------------------------------------------------\n");
}
