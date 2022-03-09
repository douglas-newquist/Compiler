#include "token.h"

#ifndef Symbol
#define Symbol struct symbol
#define SymbolTable struct symboltable

Symbol
{
	int id;
	char *string;
	int type;
	int attributes;
	Token *token;
	// Scope this symbol belongs
	SymbolTable *table;
};

Symbol *create_symbol(Token *token, char *string, int type);
void free_symbols();
void print_symbols(SymbolTable *table);
#endif
