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

	if (mode & LOCAL_SYMBOLS)
	{
		// TODO
	}

	if (mode & GLOBAL_SYMBOLS)
	{
		if (table_contains(table->parent, symbol, LOCAL_SYMBOLS | GLOBAL_SYMBOLS))
			return TRUE;
	}

	return FALSE;
}

Symbol *add_symbol(Token *token, int type)
{
	Symbol *symbol = create_symbol(token, token->text, type);
	symbol->table = scope;

	if (table_contains(scope, symbol, LOCAL_SYMBOLS))
	{
		yytext = token->text;
		line = token->line;
		column = token->column;
		error(SEMATIC_ERROR, "Multiple definitions");
	}

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

void scan_children(Tree *tree)
{
	for (int i = 0; i < tree->count; i++)
		generate_symboltable(tree->children[i]);
}

SymbolTable *generate_symboltable(Tree *tree)
{
	if (tree == NULL)
		return NULL;

	if (scope == NULL)
		enter_scope();

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_CLASS_GROUP:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_CLASS1:
		add_symbol(tree->token, S_Class);

		enter_scope();

		scan_children(tree);
		return exit_scope();

	case R_DEFINE1:
		add_symbol(tree->token, S_Variable);
		return NULL;

	case R_DEFINE3:
		add_symbol(tree->token, S_Variable);
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
			else
				generate_symboltable(tree->children[i]);
		}
		return NULL;

	case R_WHILE:
		generate_symboltable(tree->children[0]);

		enter_scope();
		generate_symboltable(tree->children[1]);
		return exit_scope();

	default:
		scan_children(tree);
		return NULL;
	}

	return exit_scope();
}
