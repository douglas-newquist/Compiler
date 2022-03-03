#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
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

	if (mode & GLOBAL_SYMBOLS)
		if (table_contains(table->parent, symbol, LOCAL_SYMBOLS | GLOBAL_SYMBOLS))
			return TRUE;

	// TODO Search local and children

	return FALSE;
}

Symbol *add_symbol(Token *token, int type)
{
	Symbol *symbol = create_symbol(token, token->text, type);
	symbol->table = scope;

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
	if (tree == NULL || tree->rule < 1000)
		return NULL;

	if (scope == NULL)
		enter_scope();

	Symbol *symbol;

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case LITERAL_BOOL:
	case LITERAL_CHAR:
	case LITERAL_DOUBLE:
	case LITERAL_INT:
	case LITERAL_NULL:
	case LITERAL_STRING:
		return NULL;

	case R_ACCESS1:
	case R_ACCESS2:
	case R_ACCESS3:
	case R_ARG_DEF_GROUP:
	case R_ARG_GROUP:
	case R_ARRAY1:
	case R_ARRAY2:
	case R_ASSIGN:
	case R_BINARY_OP:
	case R_BREAK1:
	case R_BREAK2:
	case R_CALL1:
	case R_CALL2:
	case R_CASE_GROUP:
	case R_DEFAULT_CASE:
	case R_DEFINE2:
	case R_EXP_GROUP:
	case R_IF1:
	case R_IF2:
	case R_IF3:
	case R_IF4:
	case R_METHOD2:
	case R_NEW1:
	case R_RETURN1:
	case R_RETURN2:
	case R_UNARY_OP1:
	case R_UNARY_OP2:
		scan_children(tree);
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
	case R_DEFINE3:
		add_symbol(tree->token, S_Variable);
		return NULL;

	case R_FOR:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_METHOD1:
		generate_symboltable(tree->children[0]);
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
		for (int i = 0; i < tree->count; i += 1)
		{
			if (tree->children[i]->rule == ID)
				add_symbol(tree->children[i], S_Variable);
			else
				generate_symboltable(tree->children[i]);
		}
		return NULL;

	case R_WHILE:
		generate_symboltable(tree->children[0]);

		enter_scope();
		scan_children(tree);
		return exit_scope();

	default:
		printf("Rule %d not implemented\n", tree->rule);
		return NULL;
	}

	return exit_scope();
}
