/*
	Douglas Newquist
*/

#include "hash.h"
#include "list.h"
#include "symbol.h"
#include "tree.h"

#ifndef _SYMBOLTABLE
#define _SYMBOLTABLE
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
int table_contains(SymbolTable *table, Symbol *symbol, int mode);

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
int lookup(SymbolTable *table, char *symbol, int mode);
#endif
