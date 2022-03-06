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

SymbolTable *create_symboltable(SymbolTable *parent)
{
	SymbolTable *table = malloc(sizeof(SymbolTable));
	table->id = table_count++;
	table->parent = parent;
	table->symbol_count = 0;
	table->symbols = NULL;
	table->child_count = 0;
	table->children = NULL;

	if (parent != NULL)
		parent->children = list_add(parent->children, table);

	tables = list_add(tables, table);

	return table;
}

void free_symboltable(void *symboltable)
{
	SymbolTable *table = (SymbolTable *)symboltable;
	free_list(table->children, NULL);
	free_list(table->symbols, NULL);
	free(table);
}

void free_symboltables()
{
	free_list(tables, free_symboltable);
	tables = NULL;
}

int table_contains(SymbolTable *table, Symbol *symbol, int mode)
{
	if (table == NULL || symbol == NULL)
		return FALSE;

	if (mode & LOCAL_SYMBOLS && table->symbols != NULL)
	{
		foreach_list(table->symbols)
		{
			Symbol *s2 = (Symbol *)element->value;

			if (strcmp(symbol->string, s2->string) == 0)
				return TRUE;
		}
	}

	if (mode & GLOBAL_SYMBOLS)
	{
		if (table_contains(table->parent, symbol, LOCAL_SYMBOLS | GLOBAL_SYMBOLS))
			return TRUE;
	}

	return FALSE;
}

/**
 * @brief Adds a symbol to the current scope
 *
 * @param token Token symbol to add
 * @param type Type of the symbol
 */
Symbol *add_symbol(Token *token, int type)
{
	Symbol *symbol = create_symbol(token, token->text, type);
	symbol->table = scope;

	if (table_contains(scope, symbol, LOCAL_SYMBOLS))
		error_at(token, SEMATIC_ERROR, "Redefined symbol");

	scope->symbol_count++;
	scope->symbols = list_add(scope->symbols, symbol);

	return symbol;
}

void enter_scope()
{
	scope = create_symboltable(scope);
}

SymbolTable *exit_scope()
{
	SymbolTable *old_scope = scope;
	scope = scope->parent;
	return old_scope;
}

/**
 * @brief Calls generate_symboltable on each child node in the given tree
 */
void scan_children(Tree *tree)
{
	for (int i = 0; i < tree->count; i++)
		generate_symboltable(tree->children[i]);
}

SymbolTable *generate_symboltable(Tree *tree)
{
	if (tree == NULL)
		return NULL;

	// Begin program scope
	if (scope == NULL)
		enter_scope();

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_CLASS1:
		add_symbol(tree->token, S_Class);

		enter_scope();
		scan_children(tree);
		exit_scope();
		break;

	case R_DEFINE1:
	case R_DEFINE3:
		add_symbol(tree->token, S_Variable);
		scan_children(tree);
		return NULL;

	case R_DEFINE2:
		if (tree->children[1]->rule == ID)
			add_symbol(tree->children[1]->token, S_Variable);

		scan_children(tree);
		return NULL;

	case R_FOR:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_METHOD1:
		add_symbol(tree->token, S_Method);

		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_METHOD3:
		add_symbol(tree->token, S_Method);
		scan_children(tree);
		return NULL;

	case R_STATEMENT_GROUP:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_SWITCH:
		return NULL;

	case R_VAR_GROUP:
		for (int i = 0; i < tree->count; i++)
		{
			if (tree->children[i]->rule == ID)
				add_symbol(tree->children[i]->token, S_Variable);

			generate_symboltable(tree->children[i]);
		}
		return NULL;

	case R_WHILE:
		generate_symboltable(tree->children[0]);

		enter_scope();
		scan_children(tree);
		return exit_scope();

	default:
#if DEBUG
		printf("Undefined rule action: %d\t%s\n", tree->rule, tree->name);
#endif
		scan_children(tree);
		return NULL;
	}

	// Exit program scope
	return exit_scope();
}
