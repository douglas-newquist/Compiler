#include "token.h"

#ifndef Symbol
#define Symbol struct symbol
#define SymbolTable struct symboltable

Symbol
{
	char *string;
	int type;
	Token *token;
	SymbolTable *table;
};

Symbol *create_symbol(Token *token, char *string, int type);
void free_symbols();
#endif
