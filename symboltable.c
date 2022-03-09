/*
	Douglas Newquist
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "attributes.h"
#include "errors.h"
#include "jzero.tab.h"
#include "list.h"
#include "main.h"
#include "rules.h"
#include "symbols.h"
#include "symboltable.h"

int table_count = 0;
List *tables = NULL;
// The current scope
SymbolTable *scope;

/**
 * @brief Hashes a given symbol
 */
int hash(void *symbol)
{
	Symbol *s = (Symbol *)symbol;
	char *str = s->string;
	register int h = 0;
	register char c;

	while ((c = *str++) != 0)
	{
		h += c & 0377;
		h *= 37;
	}

	return h < 0 ? -h : h;
}

/**
 * @brief Compares two symbols and returns 1 if equal
 */
int equals(void *symbol1, void *symbol2)
{
	if (symbol1 == symbol2)
		return TRUE;

	if (symbol1 == NULL || symbol2 == NULL)
		return FALSE;

	Symbol *s1 = (Symbol *)symbol1;
	Symbol *s2 = (Symbol *)symbol2;

	return strcmp(s1->string, s2->string) == 0;
}

SymbolTable *create_symboltable(SymbolTable *parent, char *name, int type)
{
	SymbolTable *table = malloc(sizeof(SymbolTable));
	table->id = table_count++;
	table->name = name;
	table->type = type;
	table->parent = parent;
	table->symbols = create_hashtable(8, hash, equals);
	table->children = create_list();

	if (parent != NULL)
		parent->children = list_add(parent->children, table);

	tables = list_add(tables, table);

	return table;
}

void free_symboltable(void *symboltable)
{
	SymbolTable *table = (SymbolTable *)symboltable;
	free_list(table->children, NULL);
	free_hashtable(table->symbols, NULL);
	free(table);
}

void free_symboltables()
{
	free_list(tables, free_symboltable);
	tables = NULL;
}

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
int table_contains(SymbolTable *table, Symbol *symbol, int mode)
{
	if (table == NULL || symbol == NULL)
		return FALSE;

	if (mode & LOCAL_SYMBOLS && hashtable_contains(table->symbols, symbol))
		return TRUE;

	if (mode & GLOBAL_SYMBOLS && table->parent)
		if (table_contains(table->parent, symbol, mode | LOCAL_SYMBOLS))
			return TRUE;

	if (mode & CHILD_SYMBOLS)
		foreach_list(child, table->children)
		{
			if (table_contains((SymbolTable *)(child->value), symbol, LOCAL_SYMBOLS))
				return TRUE;
		}

	return FALSE;
}

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
int lookup(SymbolTable *scope, char *symbol, int mode)
{
	return table_contains(scope, create_symbol(NULL, symbol, 0), mode);
}

/**
 * @brief Adds a symbol to the current scope
 *
 * @param token Token symbol to add
 * @param type Type of the symbol
 */
Symbol *add_symbol(char *name, Token *token, int type)
{
	Symbol *symbol = create_symbol(token, token ? token->text : name, type);
	symbol->table = scope;

	if (table_contains(scope, symbol, LOCAL_SYMBOLS))
		error_at(token, SEMATIC_ERROR, "Redefined symbol");

	hashtable_add(scope->symbols, symbol);

	return symbol;
}

SymbolTable *enter_scope(char *name, int type)
{
	scope = create_symboltable(scope, name, type);
	return scope;
}

SymbolTable *exit_scope()
{
	SymbolTable *old_scope = scope;
	scope = scope->parent;
	return old_scope;
}

SymbolTable *populate_symboltable(Tree *tree);

/**
 * @brief Calls generate_symboltable on each child node in the given tree
 */
void scan_children(Tree *tree)
{
	for (int i = 0; i < tree->count; i++)
		populate_symboltable(tree->children[i]);
}

SymbolTable *populate_symboltable(Tree *tree)
{
	if (tree == NULL)
		return NULL;

	Symbol *symbol;

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_CLASS1:
		symbol = add_symbol(NULL, tree->token, S_Class);
		enter_scope(symbol->string, S_Class);
		scan_children(tree);
		exit_scope();
		break;

	case R_DEFINE1:
	case R_DEFINE3:
		symbol = add_symbol(NULL, tree->token, S_Variable);
		scan_children(tree);
		return NULL;

	case R_DEFINE2:
		if (tree->children[1]->rule == ID)
			add_symbol(NULL, tree->children[1]->token, S_Variable);

		scan_children(tree);
		return NULL;

	case R_FOR:
		enter_scope("for", S_Block);
		scan_children(tree);
		return exit_scope();

	case R_METHOD1:
		symbol = add_symbol(NULL, tree->token, S_Method);
		enter_scope(symbol->string, S_Method);
		scan_children(tree);
		return exit_scope();

	case R_METHOD3:
		add_symbol(NULL, tree->token, S_Method);
		scan_children(tree);
		return NULL;

	case R_STATEMENT_GROUP:
		enter_scope("Block", S_Block);
		scan_children(tree);
		return exit_scope();

	case R_SWITCH:
		return NULL;

	case R_VAR_GROUP:
		for (int i = 0; i < tree->count; i++)
		{
			if (tree->children[i]->rule == ID)
				add_symbol(NULL, tree->children[i]->token, S_Variable);

			populate_symboltable(tree->children[i]);
		}
		return NULL;

	case R_WHILE:
		populate_symboltable(tree->children[0]);

		enter_scope("while", S_Block);
		scan_children(tree);
		return exit_scope();

	case R_ACCESS1:
		// TODO Check if defined
		break;

	case ID:
		if (lookup(scope, tree->token->text, SCOPE_SYMBOLS) == FALSE)
			error_at(tree->token, SEMATIC_ERROR, "Symbol not defined");
		return NULL;

	default:
#if DEBUG
		printf("Undefined rule action: %d\t%s\n", tree->rule, tree->name);
#endif
		scan_children(tree);
		return NULL;
	}

	return NULL;
}

void populate_builtin()
{
	add_symbol("System", NULL, S_SYSTEM);
	enter_scope("System", S_Class);
	add_symbol("out", NULL, S_SYSTEM_OUT);
	enter_scope("out", S_Class);
	add_symbol("println", NULL, S_SYSTEM_OUT_PRINTLN);
	exit_scope();
	exit_scope();

	add_symbol("String", NULL, S_STRING);
}

SymbolTable *generate_symboltable(Tree *tree)
{

	if (tree == NULL)
		return NULL;

	// Begin program scope
	if (scope == NULL)
		enter_scope("Global", S_Global);

	populate_builtin();
	populate_symboltable(tree);

	// Exit program scope
	return exit_scope();
}
