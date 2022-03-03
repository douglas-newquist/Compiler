#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "main.h"
#include "rules.h"
#include "symbols.h"
#include "symboltable.h"

List *tables = NULL;
SymbolTable *scope;

SymbolTable *create_symboltable(SymbolTable *parent)
{
	SymbolTable *table = malloc(sizeof(SymbolTable));
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

void add_symbol(Token *token, int type)
{
	Symbol *symbol = create_symbol(token, token->text, type);
	symbol->table = scope;

	scope->symbol_count++;
	scope->symbols = list_add(scope->symbols, symbol);
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

	printf("Rule %d for %s\n", tree->rule, tree->name);

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_ACCESS1:
		break;

	case R_ACCESS2:
		break;

	case R_ACCESS3:
		break;

	case R_BINARY_OP:
		generate_symboltable(tree->children[0]);
		generate_symboltable(tree->children[1]);
		break;

	case R_ARG_DEF_GROUP:
		break;

	case R_ARG_GROUP:
		break;

	case R_ARRAY1:
		generate_symboltable(tree->children[0]);
		break;

	case R_ARRAY2:
		generate_symboltable(tree->children[0]);
		generate_symboltable(tree->children[1]);
		break;

	case R_ASSIGN:
		break;

	case R_BREAK1:
		break;

	case R_BREAK2:
		break;

	case R_CALL1:
		break;

	case R_CALL2:
		break;

	case R_CASE_GROUP:
		break;

	case R_CASE:
		break;

	case R_CLASS_GROUP:
		break;

	case R_CLASS1:
		enter_scope();

		add_symbol(tree->token, S_Class);
		scan_children(tree);
		return exit_scope();

	case R_DEFAULT_CASE:
		break;

	case R_DEFINE1:
		break;

	case R_DEFINE2:
		break;

	case R_DEFINE3:
		break;

	case R_EXP_GROUP:
		break;

	case R_FOR:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_IF1:
		break;

	case R_IF2:
		break;

	case R_IF3:
		break;

	case R_IF4:
		break;

	case R_METHOD1:
		generate_symboltable(tree->children[0]);
		add_symbol(tree->token, S_Method);

		enter_scope();

		add_symbol(tree->token, S_Method);
		generate_symboltable(tree->children[0]);
		generate_symboltable(tree->children[1]);
		generate_symboltable(tree->children[2]);

		return exit_scope();

	case R_METHOD2:
		generate_symboltable(tree->children[2]);
		break;

	case R_METHOD3:
		break;

	case R_NEW1:
		break;

	case R_RETURN1:
		break;

	case R_STATEMENT_GROUP:
		enter_scope();
		scan_children(tree);
		return exit_scope();

	case R_RETURN2:
	case R_UNARY_OP1:
	case R_UNARY_OP2:
		generate_symboltable(tree->children[0]);
		break;

	case R_SWITCH:
		break;

	case R_VAR_GROUP:
		scan_children(tree);
		break;

	case R_WHILE:
		generate_symboltable(tree->children[0]);

		enter_scope();
		scan_children(tree);
		return exit_scope();

	default:
		printf("Rule %d not implemented\n", tree->rule);
		break;
	}

	return NULL;
}
