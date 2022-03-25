/*
	Douglas Newquist
*/

#include "types.h"
#include "symboltable.h"
#include "token.h"

#ifndef _SYMBOL
#define _SYMBOL

#define ATR_CONST (1 << 0)
#define ATR_DEFINED (1 << 1)
#define ATR_PUBLIC (1 << 2)
#define ATR_STATIC (1 << 3)
#define ATR_BUILTIN (1 << 4)
#define ATR_VARIABLE (1 << 5)

Symbol
{
	int id;
	char *text;
	Type *type;
	int attributes;
	Token *token;
	// Scope this symbol belongs
	SymbolTable *scope;
};

Symbol *create_symbol(Token *token, char *string, Type *type);
Symbol *simple_symbol(Token *token, char *string, int type);

void print_symbols(SymbolTable *table);
void print_dot_symbols(SymbolTable *table);
#endif
