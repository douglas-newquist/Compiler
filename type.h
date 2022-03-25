#include "tree.h"
#include "symboltable.h"

#ifndef _TYPE
#define _TYPE
enum TYPES
{
	TYPE_ARRAY = 3000,
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
			// What built-in method if relevent
			int builtin;
		} method;
	} info;
};

Type *create_type(int super_type);
int type_matches(Type *t1, Type *t2);
Type *parse_type(Tree *tree);
char *type_name(Type *type);
#endif
