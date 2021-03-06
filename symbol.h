/*
	Douglas Newquist
*/

#ifndef _SYMBOL
#define _SYMBOL

#define Symbol struct symbol

#include "address.h"
#include "type.h"
#include "symboltable.h"
#include "token.h"

#define ATR_CONST (1 << 0)
#define ATR_DEFINED (1 << 1)
#define ATR_PUBLIC (1 << 2)
#define ATR_STATIC (1 << 3)
#define ATR_BUILTIN (1 << 4)
#define ATR_VARIABLE (1 << 5)
#define ATR_PROPERTY (1 << 6)
#define ATR_RUNTIME (1 << 7)

Symbol
{
	int id;
	Address *address;
	char *start_label;
	char *end_label;
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
