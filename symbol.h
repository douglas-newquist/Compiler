#include "token.h"
#include "type.h"

#ifndef Symbol
#define Symbol struct symbol
#define SymbolTable struct symboltable

Symbol
{
	int id;
	char *string;
	Type *type;
	int attributes;
	Token *token;
	// Scope this symbol belongs
	SymbolTable *table;
};

Symbol *create_symbol(Token *token, char *string, Type *type);
Symbol *simple_symbol(Token *token, char *string, int type);
void free_symbols();
void print_symbols(SymbolTable *table);
void print_dot_symbols(SymbolTable *table);
#endif
