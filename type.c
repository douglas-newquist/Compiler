/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <string.h>
#include "jzero.tab.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"
#include "symbol.h"
#include "symbols.h"
#include "tree.h"
#include "type.h"

Type *create_type(int basetype)
{
	Type *type = alloc(sizeof(Type));
	type->base = basetype;

	switch (basetype)
	{
	case TYPE_METHOD:
		type->info.method.scope = NULL;
		type->info.method.params = NULL;
		type->info.method.result = NULL;
		type->info.method.count = 0;
		type->info.method.builtin = 0;
		break;

	case TYPE_CLASS:
		type->info.class.name = NULL;
		type->info.class.scope = NULL;
		break;

	case TYPE_ARRAY:
		type->info.array.size = -1;
		type->info.array.type = NULL;
		break;
	}

	return type;
}

int type_matches(Type *t1, Type *t2)
{
	if (t1 == t2)
		return TRUE;

	if (t1 == NULL || t2 == NULL)
		return FALSE;

	if (t1->base != t2->base)
		return FALSE;

	// TODO type check

	return FALSE;
}

Type *parse_type(SymbolTable *scope, Tree *tree)
{
	Type *type;
	Symbol *symbol;

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
		type->info.class.name = tree->token->text;
		return type;

	case R_DEFINE1:
		return parse_type(scope, tree->children[0]);

	case R_METHOD2:
		return parse_type(scope, tree->children[2]);

	case R_METHOD1:
		type = create_type(TYPE_METHOD);
		type->info.method.result = parse_type(scope, tree->children[0]);

		switch (tree->children[1]->rule)
		{
		case R_ARG_DEF_GROUP:
			type->info.method.params = alloc(sizeof(Type *) * tree->count);
			type->info.method.count = tree->count;

			for (int i = 0; i < tree->count - 1; i++)
				type->info.method.params[i] = parse_type(scope, tree->children[1]->children[i]);
			break;

		case R_DEFINE1:
			type->info.method.params = alloc(sizeof(Type *));
			type->info.method.count = 1;
			type->info.method.params[0] = parse_type(scope, tree->children[1]->children[0]);
			break;
		}

		return type;

	case R_ACCESS1:
		// TODO
		break;

	case ID:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		if (symbol == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Unknown type");
		return symbol->type;

	case R_ARRAY1:
		type = create_type(TYPE_ARRAY);
		type->info.array.type = parse_type(scope, tree->children[0]);
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

	switch (type->base)
	{
	case TYPE_INT:
		return "int";

	case TYPE_BOOL:
		return "boolean";

	case TYPE_CHAR:
		return "char";

	case TYPE_DOUBLE:
		return "double";

	case TYPE_CLASS:
		return type->info.class.name;

	case TYPE_VOID:
		return "void";

	case TYPE_NULL:
		return "null";

	case TYPE_ARRAY:
		s2 = type_name(type->info.array.type);
		s1 = alloc(sizeof(char) * (3 + 32 + strlen(s2)));

		if (type->info.array.size > 0)
			sprintf(s1, "%s[%d]", s2, type->info.array.size);
		else
			sprintf(s1, "%s[]", s2);

		return s1;

	case TYPE_METHOD:
		s1 = alloc(sizeof(char) * 4096);
		sprintf(s1, "%s(", type_name(type->info.method.result));
		int count = 0;
		for (int i = 0; i < type->info.method.count; i++)
		{
			if (count > 0)
				sprintf(s1, "%s, %s", s1, type_name(type->info.method.params[i]));
			else
				sprintf(s1, "%s%s", s1, type_name(type->info.method.params[i]));
			count++;
		}
		sprintf(s1, "%s)", s1);
		return s1;

	case TYPE_UNKNOWN:
		return "<UNKNOWN>";

	default:
#ifdef DEBUG
		printf("Unhandled type name %d\n", type->base);
#endif
		return "";
	}
}
