/*
	Douglas Newquist
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "attributes.h"
#include "errors.h"
#include "id.h"
#include "jzero.tab.h"
#include "list.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"
#include "symbol.h"
#include "symbols.h"
#include "symboltable.h"
#include "type.h"

List *tables = NULL;
// The current scope
SymbolTable *scope;

/**
 * @brief Hashes a given symbol
 */
int hash(void *symbol)
{
	Symbol *s = (Symbol *)symbol;
	char *str = s->text;
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

	return strcmp(s1->text, s2->text) == 0;
}

SymbolTable *create_symboltable(SymbolTable *parent, char *name, int type)
{
	SymbolTable *table = alloc(sizeof(SymbolTable));
	table->id = next_id();
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

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
Symbol *table_contains(SymbolTable *table, Symbol *symbol, int mode)
{
	if (table == NULL || symbol == NULL)
		return NULL;

	Symbol *result = NULL;

	if (mode & LOCAL_SYMBOLS)
	{
		void *tmp = hashtable_contains(table->symbols, symbol);
		if (tmp != NULL)
			return (Symbol *)tmp;
	}

	if (mode & GLOBAL_SYMBOLS && table->parent)
	{
		result = table_contains(table->parent, symbol, mode | LOCAL_SYMBOLS);
		if (result != NULL)
			return result;
	}

	if (mode & CHILD_SYMBOLS)
		foreach_list(child, table->children)
		{
			result = table_contains((SymbolTable *)(child->value), symbol, LOCAL_SYMBOLS);
			if (result != NULL)
				return result;
		}

	return NULL;
}

/**
 * @brief Checks if the given table contains a symbol
 *
 * @param table Table to search
 * @param symbol Symbol to search for
 * @param mode What scope(s) to search in
 */
Symbol *lookup(SymbolTable *scope, char *symbol, int mode)
{
	return table_contains(scope, create_symbol(NULL, symbol, 0), mode);
}

/**
 * @brief
 *
 * @param table Table to search
 * @param name
 * @param mode What scope(s) to search in
 */
Symbol *lookup_name(SymbolTable *scope, Tree *name, int mode)
{
	if (name == NULL)
		return NULL;

	Symbol *symbol = NULL;

	switch (name->rule)
	{
	case R_ACCESS1:
		symbol = lookup_name(scope, name->children[0], mode);
		if (symbol == NULL)
			return NULL;
		return lookup(symbol->scope, name->token->text, mode | CHILD_SYMBOLS);

	case ID:
		return lookup(scope, name->token->text, mode);
	}

	return NULL;
}

/**
 * @brief Adds a symbol to the current scope
 *
 * @param token Token symbol to add
 * @param type Type of the symbol
 */
Symbol *add_symbol(Symbol *symbol)
{
	symbol->scope = scope;

	if (table_contains(scope, symbol, LOCAL_SYMBOLS))
		error_at(symbol->token, SEMATIC_ERROR, "Redefined symbol");

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

/**
 * @brief Gets a child scope with the given name
 */
SymbolTable *get_sub_scope(SymbolTable *scope, char *name)
{
	if (scope == NULL)
		return NULL;

	foreach_list(children, scope->children)
	{
		SymbolTable *child = (SymbolTable *)children->value;
		if (strcmp(name, child->name) == 0)
			return child;
	}

	return NULL;
}

SymbolTable *populate_symboltable(Tree *tree);

/**
 * @brief Calls populate_symboltable on each child node in the given tree
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
	Tree *names;
	Type *type;

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_CLASS1:
		symbol = simple_symbol(tree->token, NULL, TYPE_CLASS);
		symbol->scope = scope;
		symbol->type->info.class.name = tree->token->text;
		symbol->attributes |= ATR_PUBLIC;
		add_symbol(symbol);
		enter_scope(symbol->text, TYPE_CLASS);
		symbol->type->info.class.scope = scope;
		scan_children(tree);
		exit_scope();
		break;

	case R_DEFINE1:
		symbol = create_symbol(tree->token,
							   tree->token->text,
							   parse_type(scope, tree->children[0]));
		symbol->scope = scope;
		symbol->attributes |= ATR_DEFINED | ATR_VARIABLE;
		add_symbol(symbol);
		return NULL;

	case R_DEFINE2:
		names = tree->children[1];
		type = parse_type(scope, tree->children[0]);

		switch (names->rule)
		{
		case ID:
			symbol = create_symbol(names->token,
								   names->token->text,
								   type);
			symbol->scope = scope;
			symbol->attributes |= ATR_VARIABLE;
			add_symbol(symbol);
			break;

		case R_DEFINE3:
			symbol = create_symbol(names->token,
								   names->token->text,
								   type);
			symbol->scope = scope;
			symbol->attributes |= ATR_DEFINED | ATR_VARIABLE;
			add_symbol(symbol);
			break;

		case R_VAR_GROUP:
			for (int i = 0; i < names->count; i++)
			{
				symbol = create_symbol(names->children[i]->token,
									   names->children[i]->token->text,
									   type);

				symbol->scope = scope;
				symbol->attributes |= ATR_VARIABLE;

				if (names->children[i]->rule == R_DEFINE3)
					symbol->attributes |= ATR_DEFINED;

				add_symbol(symbol);
			}
			break;
		}

		return NULL;

	case R_METHOD1:
		symbol = create_symbol(tree->token,
							   tree->token->text,
							   parse_type(scope, tree));
		symbol->scope = scope;
		add_symbol(symbol);
		enter_scope(symbol->text, TYPE_METHOD);
		symbol->type->info.method.scope = scope;
		scan_children(tree);
		return exit_scope();

	case R_VAR_GROUP:
		for (int i = 0; i < tree->count; i++)
		{
			if (tree->children[i]->rule == ID)
			{
				symbol = simple_symbol(tree->children[i]->token, NULL, S_Variable);
				symbol->scope = scope;
				add_symbol(symbol);
			}

			populate_symboltable(tree->children[i]);
		}
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

SymbolTable *check_defined(SymbolTable *scope, Tree *tree)
{
	Symbol *symbol = lookup_name(scope, tree, SCOPE_SYMBOLS);

	// Check that symbol is defined at all
	if (symbol == NULL)
		error_at(tree->token, SEMATIC_ERROR, "Symbol not defined");

	// Check if symbol is defined in a valid location
	if (symbol->attributes & ATR_VARIABLE)
	{
		if (symbol->token->id > tree->token->id)
			error_at(tree->token, SEMATIC_ERROR, "Variable used before definition");
	}

	return symbol->scope;
}

/**
 * @brief
 */
void check_table(SymbolTable *scope, Tree *tree)
{
	if (tree == NULL)
		return;

	switch (tree->rule)
	{
	case R_CLASS1:
	case R_METHOD1:
		//scope = lookup(scope, tree->token->text, SCOPE_SYMBOLS)->scope;
		scope = get_sub_scope(scope, tree->token->text);
		break;

	case ID:
	case R_ACCESS1:
		check_defined(scope, tree);
		return;

	case R_OP1_DECREMENT:
	case R_OP1_INCREMENT:
	case R_OP1_NEGATE:
	case R_OP1_NOT:
		set_pos(tree->children[0]->token);
		check_types(tree->rule, 1,
					parse_type(scope, tree->children[0]), NULL);
		break;

	case R_DEFINE3:
		set_pos(tree->token);
		check_types(tree->rule, 1,
					parse_type(scope, tree),
					parse_type(scope, tree->children[0]));
		break;

	case R_ASSIGN:
	case R_OP2_ADD:
	case R_OP2_AND:
	case R_OP2_DIV:
	case R_OP2_EQUALS:
	case R_OP2_GREATER_EQUAL:
	case R_OP2_GREATER:
	case R_OP2_LESS_EQUAL:
	case R_OP2_LESS:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_NOT_EQUAL:
	case R_OP2_OR:
	case R_OP2_SUB:
		set_pos(tree->token);
		check_types(tree->rule, 2,
					parse_type(scope, tree->children[0]),
					parse_type(scope, tree->children[1]));
		break;
	}

	for (int i = 0; i < tree->count; i++)
		check_table(scope, tree->children[i]);
}

Symbol *add_builtin(char *name, int type)
{
	Symbol *symbol = add_symbol(simple_symbol(NULL, name, type));
	symbol->scope = scope;
	symbol->attributes |= ATR_BUILTIN;
	return symbol;
}

void populate_builtin()
{
	Symbol *symbol = NULL;

	symbol = add_builtin("String", TYPE_CLASS);
	enter_scope(symbol->text, TYPE_CLASS);
	symbol->type->info.class.name = symbol->text;
	symbol->type->info.class.scope = scope;

	Type *string = create_type(TYPE_CLASS);
	string->info.class.name = symbol->text;
	string->info.class.scope = symbol->scope;

	symbol = add_builtin("charAt", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_CHARAT;
	symbol->type->info.method.result = create_type(TYPE_CHAR);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = simple_param("a1", TYPE_INT);

	symbol = add_builtin("equals", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_EQUALS;
	symbol->type->info.method.result = create_type(TYPE_BOOL);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("compareTo", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_COMPARETO;
	symbol->type->info.method.result = create_type(TYPE_INT);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("join", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_JOIN;
	symbol->type->info.method.result = string;
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("length", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_LENGTH;
	symbol->type->info.method.result = create_type(TYPE_INT);

	symbol = add_builtin("substring", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_SUBSTRING;
	symbol->type->info.method.result = string;
	alloc_params(symbol->type, 2);
	symbol->type->info.method.params[0] = simple_param("a1", TYPE_INT);
	symbol->type->info.method.params[1] = simple_param("a2", TYPE_INT);

	symbol = add_builtin("valueOf", TYPE_METHOD);
	symbol->type->info.method.builtin = S_STRING_VALUEOF;

	exit_scope();

	symbol = add_builtin("System", TYPE_CLASS);
	enter_scope(symbol->text, TYPE_CLASS);
	symbol->type->info.class.name = symbol->text;
	symbol->type->info.class.scope = scope;

	symbol = add_builtin("exit", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_EXIT;
	symbol->type->info.method.result = create_type(TYPE_VOID);

	symbol = add_builtin("out", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_OUT;

	symbol = add_builtin("err", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_ERR;

	enter_scope("out", TYPE_CLASS);

	symbol = add_builtin("println", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_OUT_PRINTLN;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("print", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_OUT_PRINT;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	exit_scope();

	enter_scope("err", TYPE_CLASS);

	symbol = add_builtin("println", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_ERR_PRINTLN;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("print", TYPE_METHOD);
	symbol->type->info.method.builtin = S_SYSTEM_ERR_PRINT;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	exit_scope();

	exit_scope();

	symbol = add_builtin("InputStream", TYPE_CLASS);
	enter_scope(symbol->text, TYPE_CLASS);
	symbol->type->info.class.name = symbol->text;
	symbol->type->info.class.scope = scope;

	symbol = add_builtin("read", TYPE_METHOD);
	symbol->type->info.method.builtin = S_INPUTSTREAM_READ;

	symbol = add_builtin("close", TYPE_METHOD);
	symbol->type->info.method.builtin = S_INPUTSTREAM_CLOSE;

	exit_scope();

	symbol = add_builtin("PrintStream", S_PRINTSTREAM);
	enter_scope("PrintStream", TYPE_CLASS);
	symbol->type = create_type(TYPE_CLASS);
	symbol->type->info.class.name = symbol->text;
	symbol->type->info.class.scope = scope;

	symbol = add_builtin("println", TYPE_METHOD);
	symbol->type->info.method.builtin = S_PRINTSTREAM_PRINTLN;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	symbol = add_builtin("print", TYPE_METHOD);
	symbol->type->info.method.builtin = S_PRINTSTREAM_PRINT;
	symbol->type->info.method.result = create_type(TYPE_VOID);
	alloc_params(symbol->type, 1);
	symbol->type->info.method.params[0] = create_param("a1", string);

	exit_scope();

	symbol = add_builtin("Array", TYPE_CLASS);
	enter_scope(symbol->text, TYPE_CLASS);
	symbol->type->info.class.name = symbol->text;
	symbol->type->info.class.scope = scope;

	symbol = add_builtin("length", TYPE_METHOD);
	symbol->type->info.method.builtin = S_ARRAY_LENGTH;

	exit_scope();
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
	SymbolTable *table = exit_scope();

	check_table(table, tree);

	return table;
}
