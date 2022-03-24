#include "tree.h"

#ifndef Type
#define Type struct type
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
	int super;
	int size;
	Type *subtype;
	char *string;
};

Type *create_type(int super_type);
int type_matches(Type *t1, Type *t2);
Type *parse_type(Tree *tree);
char *type_name(Type *type);
#endif
