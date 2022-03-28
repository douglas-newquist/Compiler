/*
	Douglas Newquist
*/

#ifndef _SYMBOLTABLE
#define _SYMBOLTABLE

#define SymbolTable struct symboltable

#include "hash.h"
#include "list.h"
#include "symbol.h"
#include "tree.h"

SymbolTable
{
	int id;
	// Name of this table
	char *name;
	// Type of this table
	int type;
	SymbolTable *parent;
	HashTable *symbols;
	// Child tables
	List *children;
};

#define LOCAL_SYMBOLS (1)
#define GLOBAL_SYMBOLS (1 << 2)
#define SCOPE_SYMBOLS (LOCAL_SYMBOLS | GLOBAL_SYMBOLS)
#define CHILD_SYMBOLS (1 << 3)

SymbolTable *generate_symboltable(Tree *program);
void free_symboltables();

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
Symbol *lookup(SymbolTable *table, char *symbol, int mode);
#endif
