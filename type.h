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
	TYPE_STRING,
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

Type *create_type(int super_type);
int type_matches(Type *t1, Type *t2);
Type *parse_type(SymbolTable *scope, Tree *tree);
char *type_name(Type *type);
#endif
