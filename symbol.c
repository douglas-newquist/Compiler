#include <stdio.h>
#include <stdlib.h>
#include "id.h"
#include "list.h"
#include "symbol.h"
#include "symboltable.h"

List *symbols = NULL;

Symbol *create_symbol(Token *token, char *string, Type *type)
{
	Symbol *symbol = malloc(sizeof(Symbol));
	symbol->id = next_id();
	symbol->string = string;
	symbol->type = type;
	symbol->attributes = 0;
	symbol->token = token;
	symbol->table = NULL;

	symbols = list_add(symbols, symbol);

	return symbol;
}

Symbol *simple_symbol(Token *token, char *string, int type)
{
	return create_symbol(token, token ? token->text : string, create_type(type));
}

void free_symbol(void *symbol)
{
	Symbol *s = (Symbol *)symbol;

	if (s->type)
		free_type(s->type);

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

	if (s == NULL)
		return;

	printf("    %s\n", s->string);
}

int indent = 0;
void print_symbols(SymbolTable *table)
{
	if (indent == 0)
		printf("---- Symbols -------------------------------------------------\n");

	indent++;

	if (table)
	{
		printf("-- Table %d, %s -- Parent %d, %s --\n",
			   table->id, table->name,
			   table->parent ? table->parent->id : -1,
			   table->parent ? table->parent->name : "NA");

		foreach_hashtable(element, table->symbols)
			print_symbol(element->value);
	}

	foreach_list(element, table->children)
		print_symbols((SymbolTable *)element->value);

	indent--;

	if (indent == 0)
		printf("--------------------------------------------------------------\n");
}

void print_dot_symbols(SymbolTable *table)
{
	if (table == NULL)
		return;

	if (indent == 0)
		printf("graph {\n");

	indent++;

	printf("\"%d %s\" [shape=box]\n", table->id, table->name);

	foreach_hashtable(element, table->symbols)
	{
		Symbol *symbol = (Symbol *)element->value;
		printf("\"%d %s\" -- \"%d %s\"\n",
			   table->id, table->name,
			   symbol->id, symbol->string);
	}

	foreach_list(element, table->children)
	{
		SymbolTable *t2 = (SymbolTable *)element->value;
		printf("\"%d %s\" -- \"%d %s\"\n",
			   table->id, table->name,
			   t2->id, t2->name);
		print_dot_symbols(t2);
	}

	indent--;

	if (indent == 0)
		printf("}");
}
