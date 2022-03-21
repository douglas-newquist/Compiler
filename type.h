#include "tree.h"

#ifndef Type
#define Type struct type
enum TYPES
{
	TYPE_ARRAY,
	TYPE_BOOL,
	TYPE_CHAR,
	TYPE_CLASS,
	TYPE_DOUBLE,
	TYPE_INT,
	TYPE_NULL,
	TYPE_OBJECT,
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
void free_type(Type *type);
int type_matches(Type *t1, Type *t2);
Type *parse_type(Tree *tree);
#endif
