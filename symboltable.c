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
	table->symbols = create_hashtable(64, hash, equals);
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

	if (name->token)
		set_pos(name->token);

	switch (name->rule)
	{
	case R_ACCESS1:
		symbol = lookup_name(scope, name->children[0], mode);
		if (symbol == NULL)
			error(SEMATIC_ERROR, "Unknown reference");
		switch (symbol->type->base)
		{
		case TYPE_ARRAY:
			scope = lookup(scope, "Array", SCOPE_SYMBOLS)->scope;
			break;

		case TYPE_CLASS:
			symbol = lookup(scope, symbol->type->info.class.name, SCOPE_SYMBOLS);
			if (symbol == NULL)
				error(SEMATIC_ERROR, error_message("Cannot find scope for %s\n",
												   symbol->type->info.class.name));
			scope = symbol->scope;
			break;

		default:
			scope = symbol->scope;
			break;
		}
		return lookup(scope, name->token->text, CHILD_SYMBOLS);

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

	symbol->offset = scope->symbols->count;
	hashtable_add(scope->symbols, symbol);

	return symbol;
}

SymbolTable *enter_scope(char *name, int type)
{
	scope = create_symboltable(scope, name, type);
	return scope;
}

/**
 * @brief Exits the current scope
 */
SymbolTable *exit_scope()
{
	SymbolTable *old_scope = scope;
	scope = scope->parent;
	return old_scope;
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

void define_variables(Tree *tree, Type *type)
{
	Symbol *symbol;

	switch (tree->rule)
	{
	case ID:
		symbol = create_symbol(tree->token,
							   tree->token->text,
							   type);
		symbol->scope = scope;
		symbol->attributes |= ATR_VARIABLE;
		add_symbol(symbol);
		break;

	case R_DEFINE3:
		symbol = create_symbol(tree->token,
							   tree->token->text,
							   type);
		symbol->scope = scope;
		symbol->attributes |= ATR_DEFINED | ATR_VARIABLE;
		add_symbol(symbol);
		break;

	case R_VAR_GROUP:
		for (int i = 0; i < tree->count; i++)
		{
			symbol = create_symbol(tree->children[i]->token,
								   tree->children[i]->token->text,
								   type);

			symbol->scope = scope;
			symbol->attributes |= ATR_VARIABLE;

			if (tree->children[i]->rule == R_DEFINE3)
				symbol->attributes |= ATR_DEFINED;

			add_symbol(symbol);
		}
		break;
	}
}

int name_matches(Token *token)
{
	char *i = strrchr(token->filename, '/');

	if (i == NULL)
		i = token->filename;
	else
		i++;

	return strncmp(i, token->text, strlen(i) - 5) == 0;
}

SymbolTable *populate_symboltable(Tree *tree)
{
	if (tree == NULL)
		return NULL;

	Symbol *symbol = NULL;
	Type *type = NULL;

	if (tree->token)
		set_pos(tree->token);

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case R_CLASS1:
		if (name_matches(tree->token) == FALSE)
			error_at(tree->token, SEMATIC_ERROR, "Class name does not match filename");
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

	case R_FIELD:
		type = parse_type(scope, tree->children[2]);
		define_variables(tree->children[3], type);
		return NULL;

	case R_DEFINE2:
	case R_DEFINE4:
		type = parse_type(scope, tree->children[0]);
		define_variables(tree->children[1], type);
		return NULL;

	case R_METHOD1:
		symbol = create_symbol(tree->token,
							   tree->token->text,
							   parse_type(scope, tree));
		add_symbol(symbol);
		symbol->type->info.method.scope = enter_scope(symbol->text, TYPE_METHOD);
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
		printf("Undefined populate rule action: %d\t%s\n", tree->rule, tree->name);
#endif
		scan_children(tree);
		return NULL;
	}

	return NULL;
}

/**
 * @brief
 */
void check_table(SymbolTable *scope, Tree *tree)
{
	if (tree == NULL)
		return;

	Symbol *symbol;
	Type *type, *t2;
	char *message;

	if (tree->token)
		set_pos(tree->token);

	switch (tree->rule)
	{
	case R_EMPTY:
		return;

	case R_CLASS1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.class.scope;
		break;

	case R_METHOD1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.method.scope;
		break;

	case ID:
	case R_ACCESS1:
		symbol = lookup_name(scope, tree, SCOPE_SYMBOLS);

		// Check that symbol is defined at all
		if (symbol == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Symbol not defined");

		// Check if symbol is defined in a valid location
		if (symbol->attributes & ATR_VARIABLE)
		{
			if (symbol->token->id > tree->token->id)
				error_at(tree->token, SEMATIC_ERROR, "Variable used before definition");
		}

		if (symbol->type == NULL)
			error_at(tree->token, SEMATIC_ERROR, "ID has no type");
		return;

	case R_ACCESS3:
		if (type_fuzzy_match(create_type(TYPE_INT),
							 parse_type(scope, tree->children[1])) == FALSE)
			error(SEMATIC_ERROR, "Array index must be an integer");
		break;

	case R_OP1_DECREMENT:
	case R_OP1_INCREMENT:
	case R_OP1_NEGATE:
	case R_OP1_NOT:
		set_pos(tree->children[0]->token);
		check_types(tree->rule, 1,
					parse_type(scope, tree->children[0]), NULL);
		break;

	case R_DEFINE3:
		check_types(tree->rule, 1,
					parse_type(scope, tree),
					parse_type(scope, tree->children[0]));
		break;

	case R_CALL1:
		set_pos(tree->children[0]->token);
		symbol = lookup_name(scope, tree->children[0], SCOPE_SYMBOLS);

		if (symbol == NULL)
			error(SEMATIC_ERROR, "Undefined method");

		type = symbol->type;

		switch (tree->children[1]->rule)
		{
			// Zero argument method call
		case R_EMPTY:
			if (type->info.method.count != 0)
				error_at(tree->token, SEMATIC_ERROR,
						 error_message("Incorrect number of arguments, got 0, expected %d",
									   type->info.method.count));
			break;

			// Multi-argument method call
		case R_ARG_GROUP:
			if (tree->children[1]->count != type->info.method.count)
				error_at(tree->token, SEMATIC_ERROR,
						 error_message("Incorrect number of arguments, got %d, expected %d",
									   tree->children[1]->count,
									   type->info.method.count));

			switch (type->info.method.builtin)
			{
			default:
				// Check input types
				for (int i = 0; i < type->info.method.count; i++)
				{
					t2 = parse_type(scope, tree->children[1]->children[i]);
					if (type_fuzzy_match(type->info.method.params[i]->type, t2) == FALSE)
					{
						// Type mismatch
						message = error_message("Paramter type mismatch, expected %s, got %s",
												type_name(type->info.method.params[i]->type),
												type_name(t2));
						error_at(tree->token, SEMATIC_ERROR, message);
					}
				}
				break;
			}
			break;

			// Single argument method call
		default:
			if (type->info.method.count != 1)
				error_at(tree->token, SEMATIC_ERROR,
						 error_message("Incorrect number of arguments, got 1, expected %d",
									   type->info.method.count));

			switch (type->info.method.builtin)
			{
				// Skip type checking for these built-in methods
			case S_PRINTSTREAM_PRINT:
			case S_PRINTSTREAM_PRINTLN:
			case S_SYSTEM_ERR_PRINT:
			case S_SYSTEM_ERR_PRINTLN:
			case S_SYSTEM_OUT_PRINT:
			case S_SYSTEM_OUT_PRINTLN:
				break;

				// Check input type
			default:
				t2 = parse_type(scope, tree->children[1]);
				if (type_fuzzy_match(type->info.method.params[0]->type, t2) == FALSE)
				{
					// Type mismatch
					message = error_message("Paramter type mismatch, expected %s, got %s",
											type_name(type->info.method.params[0]->type),
											type_name(t2));
					error_at(tree->token, SEMATIC_ERROR, message);
				}
				break;
			}
			break;
		}
		break;

	case R_ASSIGN1:
	case R_ASSIGN2:
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
		check_types(tree->rule, 2,
					parse_type(scope, tree->children[0]),
					parse_type(scope, tree->children[1]));
		break;

	case PUBLIC:
	case STATIC:
	case VOID:
		break;

#ifdef DEBUG
	default:
		printf("Undefined symbol check rule: %d\n", tree->rule);
		break;
#endif
	}

	for (int i = 0; i < tree->count; i++)
		check_table(scope, tree->children[i]);
}

/**
 * @brief Adds a new built-in symbol
 *
 * @param name Name of the symbol
 * @param type What base type is the symbol
 */
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
	symbol->attributes |= ATR_PROPERTY;
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
	symbol->attributes |= ATR_PROPERTY;
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
