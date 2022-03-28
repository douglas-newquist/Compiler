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

/**
 * @brief Creates a new type
 *
 * @param basetype The base type of this type
 */
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

/**
 * @brief Checks if two types match, accounting for automatic casting
 */
int type_fuzzy_match(Type *t1, Type *t2)
{
	if (type_matches(t1, t2) == TRUE)
		return TRUE;

	switch (t1->base)
	{
	case TYPE_ARRAY:
	case TYPE_CLASS:
		switch (t2->base)
		{
		case TYPE_NULL:
			return TRUE;
		}
		break;

	case TYPE_DOUBLE:
		switch (t2->base)
		{
		case TYPE_INT:
			return TRUE;
		}
	case TYPE_INT:
		switch (t2->base)
		{
		case TYPE_CHAR:
			return TRUE;
		}
		break;
	}

	return FALSE;
}

/**
 * @brief Checks if two types exactly match
 */
int type_matches(Type *t1, Type *t2)
{
	if (t1 == t2)
		return TRUE;

	if (t1 == NULL || t2 == NULL)
		return FALSE;

	if (t1->base != t2->base)
		return FALSE;

	switch (t1->base)
	{
	case TYPE_METHOD:
		// TODO type check
		break;

	case TYPE_CLASS:
		return strcmp(t1->info.class.name, t2->info.class.name) == 0;

	case TYPE_ARRAY:
		return type_matches(t1->info.array.type, t2->info.array.type);
	}

	return FALSE;
}

/**
 * @brief Parses a type from the given tree structure
 *
 * @param scope Scope to constrain to
 * @param tree Tree to search
 */
Type *parse_type(SymbolTable *scope, Tree *tree)
{
	Type *type;
	Symbol *symbol;

	switch (tree->rule)
	{
	case BOOLEAN:
	case LITERAL_BOOL:
	case R_OP2_AND:
	case R_OP2_EQUALS:
	case R_OP2_GREATER:
	case R_OP2_LESS:
	case R_OP2_NOT_EQUAL:
	case R_OP2_OR:
		return create_type(TYPE_BOOL);

	case R_OP2_ADD:
	case R_OP2_DIV:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_SUB:
		return parse_type(scope, tree->children[0]);

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
		return lookup(scope, "String", SCOPE_SYMBOLS)->type;

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
			alloc_params(type, tree->children[1]->count);

			for (int i = 0; i < type->info.method.count; i++)
				type->info.method.params[i] = create_param(
					tree->children[1]->children[i]->token->text,
					parse_type(scope, tree->children[1]->children[i]));
			break;

		case R_DEFINE1:
			alloc_params(type, 1);
			type->info.method.params[0] = create_param(
				tree->children[1]->token->text,
				parse_type(scope, tree->children[1]->children[0]));
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

/**
 * @brief Gets the name of the type
 */
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
		s1 = alloc(sizeof(char) * (3 + strlen(s2)));

		sprintf(s1, "%s[]", s2);

		return s1;

	case TYPE_METHOD:
		s1 = alloc(sizeof(char) * 4096);
		sprintf(s1, "%s(", type_name(type->info.method.result));
		int count = 0;
		for (int i = 0; i < type->info.method.count; i++)
		{
			if (count > 0)
				strcat(s1, ", ");
			strcat(s1, type_name(type->info.method.params[i]->type));
#ifdef DEBUG
			strcat(s1, " ");
			strcat(s1, type->info.method.params[i]->name);
#endif
			count++;
		}
		strcat(s1, ")");
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

int check_types(int op, Type *t1, Type *t2)
{
	if (t1 == NULL || t2 == NULL)
		error(SEMATIC_ERROR, "Expected a type");

	switch (op)
	{
	case R_OP2_AND:
	case R_OP2_OR:
		if (t1->base != TYPE_BOOL)
			error(SEMATIC_ERROR, "Expected boolean type for first value");
		if (t2->base != TYPE_BOOL)
			error(SEMATIC_ERROR, "Expected boolean type for second value");
		return TRUE;

	case R_OP2_EQUALS:
	case R_OP2_NOT_EQUAL:
		return TRUE;

	case R_OP2_ADD:
	case R_OP2_DIV:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_SUB:
		if (t1->base == TYPE_INT ||
			t1->base == TYPE_DOUBLE)
		{
			if (type_fuzzy_match(t1, t2) == TRUE)
				return TRUE;

			error(SEMATIC_ERROR, "");
		}
		else
			error(SEMATIC_ERROR, "");

	default:
		printf("Undefined op type %d\n", op);
		break;
	}

	return FALSE;
}
