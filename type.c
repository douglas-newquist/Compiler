/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <string.h>
#include "jzero.tab.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"
#include "symbols.h"
#include "tree.h"
#include "type.h"

Type *create_type(int super_type)
{
	Type *type = alloc(sizeof(Type));
	type->super = super_type;
	type->size = 0;
	type->subtype = NULL;
	return type;
}

int type_matches(Type *t1, Type *t2)
{
	if (t1 == t2)
		return TRUE;

	if (t1 == NULL || t2 == NULL)
		return FALSE;

	if (t1->super != t2->super)
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

	case R_CLASS1:
		type = create_type(TYPE_CLASS);
		type->string = tree->token->text;
		return type;

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

char *type_name(Type *type)
{
	char *s1, *s2;

	if (type == NULL)
		return "<NULL>";

	switch (type->super)
	{
	case S_Variable:
		return type_name(type->subtype);

	case TYPE_INT:
		return "int";

	case TYPE_BOOL:
		return "boolean";

	case TYPE_CHAR:
		return "char";

	case TYPE_DOUBLE:
		return "double";

	case TYPE_CLASS:
		return type->string;

	case TYPE_VOID:
		return "void";

	case TYPE_NULL:
		return "null";

	case TYPE_ARRAY:
		s2 = type_name(type->subtype);
		s1 = alloc(sizeof(char) * (3 + 32 + strlen(s2)));

		if (type->size == 0)
			sprintf(s1, "%s[]", s2);
		else
			sprintf(s1, "%s[%d]", s2, type->size);

		return s1;

	case TYPE_METHOD:
		return "method";

	default:
#ifdef DEBUG
		printf("Unhandled type name %d\n", type->super);
#endif
		return "";
	}
}
