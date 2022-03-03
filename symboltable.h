#include "list.h"
#include "symbol.h"
#include "tree.h"

#ifndef _SYMBOLTABLE
#define _SYMBOLTABLE
SymbolTable
{
	SymbolTable *parent;
	int symbol_count;
	List *symbols;
	int child_count;
	List *children;
};

#define LOCAL_SYMBOLS (1)
#define GLOBAL_SYMBOLS (1 << 2)
#define CHILD_SYMBOLS (1 << 3)
#define SCOPE_SYMBOLS (LOCAL_SYMBOLS | GLOBAL_SYMBOLS)
#define ALL_SYMBOLS (SCOPE_SYMBOLS | CHILD_SYMBOLS)

SymbolTable *generate_symboltable(Tree *program);
void free_symboltables();

int table_contains(SymbolTable *table, Symbol *symbol, int mode);
#endif
