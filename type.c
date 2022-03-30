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
		switch (t2->base)
		{
		case TYPE_NULL:
			return TRUE;
		}
		break;

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

	if (t1->base == t2->base)
	{
		switch (t1->base)
		{
		case TYPE_BOOL:
		case TYPE_CHAR:
		case TYPE_DOUBLE:
		case TYPE_INT:
		case TYPE_NULL:
		case TYPE_UNKNOWN:
		case TYPE_VOID:
			return TRUE;

		case TYPE_METHOD:
			// TODO type check
			break;

		case TYPE_CLASS:
			return strcmp(t1->info.class.name, t2->info.class.name) == 0;

		case TYPE_ARRAY:
			return type_matches(t1->info.array.type, t2->info.array.type);
		}
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

	if (tree == NULL)
		return NULL;

	if (tree->token)
		set_pos(tree->token);

	switch (tree->rule)
	{
	case BOOLEAN:
	case LITERAL_BOOL:
	case R_OP1_NOT:
	case R_OP2_AND:
	case R_OP2_EQUALS:
	case R_OP2_GREATER_EQUAL:
	case R_OP2_GREATER:
	case R_OP2_LESS_EQUAL:
	case R_OP2_LESS:
	case R_OP2_NOT_EQUAL:
	case R_OP2_OR:
		return create_type(TYPE_BOOL);

	case R_OP1_DECREMENT:
	case R_OP1_INCREMENT:
	case R_OP1_NEGATE:
	case R_OP2_ADD:
	case R_OP2_DIV:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_SUB:
	case R_RETURN2:
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

	case R_CALL1:
		set_pos(tree->children[0]->token);
		symbol = lookup_name(scope, tree->children[0], SCOPE_SYMBOLS);
		type = symbol->type;

		if (symbol == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Unknown method");
		if (type == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Method has no type");
		if (type->base != TYPE_METHOD)
			error_at(tree->token, SEMATIC_ERROR, "Symbol is not a method");

		return type->info.method.result;

	case R_METHOD2:
		return parse_type(scope, tree->children[2]);

	case R_ACCESS1:
		symbol = lookup_name(scope, tree, SCOPE_SYMBOLS);
		if (symbol == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Unknown reference");
		if (symbol->attributes & ATR_PROPERTY)
			return symbol->type->info.method.result;
		return symbol->type;

	case ID:
	case R_DEFINE3:
	case R_METHOD3:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		if (symbol == NULL)
			error_at(tree->token, SEMATIC_ERROR, "Unknown type");
		if (symbol->attributes & ATR_PROPERTY)
			return symbol->type->info.method.result;
		return symbol->type;

	case R_ARRAY1:
		type = create_type(TYPE_ARRAY);
		type->info.array.type = parse_type(scope, tree->children[0]);
		return type;

	case R_RETURN1:
	case VOID:
		return create_type(TYPE_VOID);

#ifdef DEBUG
	default:
		printf("No type parse rule for %d\n", tree->rule);
		break;
#endif
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

#ifdef DEBUG
	default:
		printf("Unhandled type name %d\n", type->base);
		break;
#endif
	}

	return "";
}

int check_types(int op, int argc, Type *t1, Type *t2)
{
	if (t1 == NULL || (argc > 1 && t2 == NULL))
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

	case R_OP1_NEGATE:
		switch (t1->base)
		{
		case TYPE_INT:
		case TYPE_DOUBLE:
			return TRUE;
		}
		error(SEMATIC_ERROR, error_message("Cannot negate %s", type_name(t1)));
		return FALSE;

	case R_OP1_INCREMENT:
	case R_OP1_DECREMENT:
		switch (t1->base)
		{
		case TYPE_CHAR:
		case TYPE_DOUBLE:
		case TYPE_INT:
			return TRUE;
		}
		error(SEMATIC_ERROR, error_message("STEP not supported for %s", type_name(t1)));
		return FALSE;

	case R_OP1_NOT:
		if (t1->base == TYPE_BOOL)
			return TRUE;
		error(SEMATIC_ERROR, error_message("NOT not supported for %s", type_name(t1)));
		return FALSE;

	case R_OP2_EQUALS:
	case R_OP2_NOT_EQUAL:
		if (t1->base == t2->base)
			return TRUE;

		switch (t1->base)
		{
		case TYPE_CHAR:
		case TYPE_DOUBLE:
		case TYPE_INT:
			switch (t2->base)
			{
			case TYPE_CHAR:
			case TYPE_DOUBLE:
			case TYPE_INT:
				return TRUE;
			}
			break;

		case TYPE_NULL:
			switch (t2->base)
			{
			case CLASS:
			case TYPE_ARRAY:
			case TYPE_NULL:
				return TRUE;
			}
			break;

		case CLASS:
			switch (t2->base)
			{
			case TYPE_NULL:
			case TYPE_CLASS:
				return TRUE;
			}
			break;
		}
		error(SEMATIC_ERROR, error_message("Compare not supported for %s", type_name(t1)));
		return FALSE;

	case R_OP2_LESS:
	case R_OP2_LESS_EQUAL:
	case R_OP2_GREATER:
	case R_OP2_GREATER_EQUAL:
		switch (t1->base)
		{
		case TYPE_CHAR:
		case TYPE_DOUBLE:
		case TYPE_INT:
			switch (t2->base)
			{
			case TYPE_CHAR:
			case TYPE_DOUBLE:
			case TYPE_INT:
				return TRUE;

			default:
				error(SEMATIC_ERROR, error_message("Cannot compare %s and %s", type_name(t1), type_name(t2)));
				break;
			}
			break;

		case TYPE_CLASS:
			break;
		}
		error(SEMATIC_ERROR, error_message("Compare not supported for %s", type_name(t1)));
		return FALSE;

	case R_OP2_ADD:
		switch (t1->base)
		{
		case TYPE_INT:
		case TYPE_DOUBLE:
			if (type_fuzzy_match(t1, t2) == TRUE)
				return TRUE;

			error(SEMATIC_ERROR, error_message("Incompatible types between %s and %s", type_name(t1), type_name(t2)));
			break;

		case TYPE_CLASS:
			if (strcmp(t1->info.class.name, "String") == 0)
				switch (t2->base)
				{
				case TYPE_BOOL:
				case TYPE_CHAR:
				case TYPE_CLASS:
				case TYPE_DOUBLE:
				case TYPE_INT:
				case TYPE_NULL:
					return TRUE;
				}
			break;
		}
		error(SEMATIC_ERROR, error_message("Invalid operation on %s", type_name(t1)));
		return FALSE;

	case R_OP2_DIV:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_SUB:
		switch (t1->base)
		{
		case TYPE_INT:
		case TYPE_DOUBLE:
			if (type_fuzzy_match(t1, t2) == TRUE)
				return TRUE;

			error(SEMATIC_ERROR, error_message("Incompatible types between %s and %s", type_name(t1), type_name(t2)));
			break;
		}
		error(SEMATIC_ERROR, error_message("Invalid operation on %s", type_name(t1)));
		return FALSE;

	case R_ASSIGN:
	case R_DEFINE3:
		if (type_fuzzy_match(t1, t2) == TRUE)
			return TRUE;
		error(SEMATIC_ERROR, error_message("Type mismatch between %s and %s", type_name(t1), type_name(t2)));
		return FALSE;

#ifdef DEBUG
	default:
		printf("Undefined op type %d\n", op);
		break;
#endif
	}

	return FALSE;
}
