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

	printf("%d\t%d\t%d\t%s\t%s\n",
		   s->type,
		   s->table ? s->table->id : -1,
		   s->table && s->table->parent ? s->table->parent->id : -1,
		   s->string,
		   s->token ? s->token->filename : "Unknown");
}

int indent = 0;
void print_symbols(SymbolTable *table)
{
	if (indent == 0)
	{
		printf("---- Symbols -------------------------------------------------\n");
		printf("Type\tTable\tParent\tName\tFile\n");
	}

	indent++;

	if (table != NULL)
		print_list(table->symbols, print_symbol);

	foreach_list(table->children)
		print_symbols((SymbolTable *)element->value);

	indent--;

	if (indent == 0)
		printf("--------------------------------------------------------------\n");
}
