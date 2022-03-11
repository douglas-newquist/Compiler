/*
	Douglas Newquist
*/

#include <stdlib.h>
#include "jzero.tab.h"
#include "main.h"
#include "tree.h"
#include "type.h"
#include "rules.h"

Type *create_type(int super_type)
{
	Type *type = malloc(sizeof(Type));
	type->super = super_type;
	type->size = 0;
	type->subtype = NULL;
	return type;
}

void free_type(Type *type)
{
	if (type->subtype)
		free_type(type->subtype);

	free(type);
}

int type_matches(Type *t1, Type *t2)
{
	if (t1 == t2)
		return TRUE;

	if (t1 == NULL || t2 == NULL)
		return FALSE;

	// TODO type check

	return FALSE;
}

Type *parse_type(Tree *tree)
{
	Type *type;

	switch (tree->rule)
	{
	case BOOLEAN:
	case LITERAL_BOOL:
		return create_type(TYPE_BOOL);

	case INT:
	case LITERAL_INT:
		return create_type(TYPE_INT);

	case DOUBLE:
	case LITERAL_DOUBLE:
		return create_type(TYPE_DOUBLE);

	case LITERAL_NULL:
		return create_type(TYPE_NULL);

	case CHAR:
	case LITERAL_CHAR:
		return create_type(TYPE_CHAR);

	case LITERAL_STRING:
		return create_type(TYPE_STRING);

	case R_ACCESS1:
		// TODO
		break;

	case R_ARRAY1:
		type = create_type(TYPE_ARRAY);
		type->subtype = parse_type(tree->children[0]);
		return type;

	case VOID:
		return create_type(TYPE_VOID);
	}

	return NULL;
}
