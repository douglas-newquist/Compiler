/*
	Douglas Newquist
*/

#ifndef _TYPE
#define _TYPE

#define Type struct type

#include "list.h"
#include "param.h"
#include "param.h"
#include "symboltable.h"
#include "tree.h"
#include "type.h"

enum TYPES
{
	TYPE_UNKNOWN = 3000,
	TYPE_ARRAY,
	TYPE_BOOL,
	TYPE_CHAR,
	TYPE_CLASS,
	TYPE_DOUBLE,
	TYPE_INT,
	TYPE_METHOD,
	TYPE_NULL,
	TYPE_VOID
};

Type
{
	int base;

	union
	{
		struct class_info
		{
			char *name;
			SymbolTable *scope;
		} class;

		struct array_info
		{
			int size;
			Type *type;
		} array;

		struct method_info
		{
			SymbolTable *scope;
			Type *result;
			int count;
			Param **params;
			// What built-in method if relevent
			int builtin;
		} method;
	} info;
};

/**
 * @brief Creates a new type
 *
 * @param basetype The base type of this type
 */
Type *create_type(int basetype);

/**
 * @brief Checks if two types match, accounting for automatic casting
 */
int type_fuzzy_match(Type *t1, Type *t2);

/**
 * @brief Checks if two types exactly match
 */
int type_matches(Type *t1, Type *t2);

/**
 * @brief Parses a type from the given tree structure
 *
 * @param scope Scope to constrain to
 * @param tree Tree to search
 */
Type *parse_type(SymbolTable *scope, Tree *tree);

/**
 * @brief Gets the name of the type
 */
char *type_name(Type *type);

int check_types(int op, int argc, Type *t1, Type *t2);
#endif
