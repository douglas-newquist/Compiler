/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "address.h"
#include "imcode.h"
#include "instruction.h"
#include "jzero.tab.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"

int i = 0;
int has_main = FALSE;
int offset = 0;

Address *populate_code(ICode *code, SymbolTable *scope, Tree *tree);

void generate_children_code(ICode *code, SymbolTable *scope, Tree *tree)
{
	if (tree == NULL)
		return;

	for (int i = 0; i < tree->count; i++)
		populate_code(code, scope, tree->children[i]);
}

void builtin_call(ICode *code, Symbol *function, Tree *tree)
{
}

Address *populate_code(ICode *code, SymbolTable *scope, Tree *tree)
{
	if (tree == NULL)
		return NULL;

	Symbol *symbol = NULL;
	Instruction *i1, *i2;
	Address *a1 = NULL, *a2 = NULL, *a3 = NULL;
	int i = 0;

	set_pos(find_nearest(tree));

	switch (tree->rule)
	{
	case ID:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);

		if (tree->token->symbol)
		{
			symbol = tree->token->symbol;
			if (symbol->attributes & ATR_VARIABLE)
				LIST_ADD(code->instructions,
						 create_instruction(I_ASSIGN, symbol->address, Const(0), NULL));
		}

		return symbol->address;

	case LITERAL_BOOL:
		return create_address(RE_CONST, tree->token->bval);

	case LITERAL_CHAR:
		return create_address(RE_CONST, tree->token->cval);

	case LITERAL_DOUBLE:
		// TODO
		return NULL;

	case LITERAL_INT:
		return create_address(RE_CONST, tree->token->ival);

	case LITERAL_STRING:
		foreach_list(string, code->strings)
		{
			if (strcmp((char *)string->value, tree->token->sval) == 0)
				return create_address(RE_STRINGS, i * BYTE_SIZE);
			i++;
		}
		LIST_ADD(code->strings, tree->token->sval);
		return create_address(RE_STRINGS, (code->strings->size - 1) * BYTE_SIZE);

	case LITERAL_NULL:
		return create_address(RE_CONST, 0);

	case R_OP1_DECREMENT:
		a1 = populate_code(code, scope, tree->children[0]);
		LIST_ADD(code->instructions, create_instruction(R_OP2_SUB, a1, a1, create_address(RE_CONST, 1)));
		return a1;

	case R_OP1_INCREMENT:
		a1 = populate_code(code, scope, tree->children[0]);
		LIST_ADD(code->instructions, create_instruction(R_OP2_ADD, a1, a1, create_address(RE_CONST, 1)));
		return a1;

	case R_OP2_ADD:
	case R_OP2_AND:
	case R_OP2_DIV:
	case R_OP2_EQUALS:
	case R_OP2_GREATER_EQUAL:
	case R_OP2_GREATER:
	case R_OP2_LESS_EQUAL:
	case R_OP2_LESS:
	case R_OP2_MOD:
	case R_OP2_MULT:
	case R_OP2_NOT_EQUAL:
	case R_OP2_OR:
	case R_OP2_SUB:
		a3 = populate_code(code, scope, tree->children[1]);
	case R_OP1_NEGATE:
	case R_OP1_NOT:
		a2 = populate_code(code, scope, tree->children[0]);
		a1 = create_address(RE_LOCAL, (offset++) * BYTE_SIZE);
		LIST_ADD(code->instructions, create_instruction(tree->rule, a1, a2, a3));
		return a1;

	case PUBLIC:
	case R_EMPTY:
	case STATIC:
		break;

	case R_ASSIGN1:
		a1 = populate_code(code, scope, tree->children[0]);
		a2 = populate_code(code, scope, tree->children[1]);
		LIST_ADD(code->instructions, create_instruction(I_ASSIGN, a1, a2, a3));
		break;

	case R_CALL1:
		symbol = lookup_name(scope, tree->children[0], SCOPE_SYMBOLS);
		switch (tree->children[1]->rule)
		{
		case R_EMPTY:
			break;

		case R_ARG_GROUP:
			for (i = tree->children[1]->count - 1; i > -1; i--)
				LIST_ADD(code->instructions,
						 create_instruction(I_PARAM,
											populate_code(code, scope, tree->children[1]->children[i]),
											NULL,
											NULL));
			break;

		default:
			LIST_ADD(code->instructions,
					 create_instruction(I_PARAM,
										populate_code(code, scope, tree->children[1]),
										NULL,
										NULL));
			break;
		}

		if (symbol->attributes & ATR_BUILTIN)
		{
			builtin_call(code, symbol, tree);
			a1 = create_label_address(symbol->text);
		}
		else
		{
			a1 = create_label_address(symbol->start_label);
		}

		a2 = create_address(RE_CONST, symbol->type->info.method.count);
		a3 = create_address(RE_LOCAL, (offset++) * BYTE_SIZE);
		LIST_ADD(code->instructions, create_instruction(I_CALL, a1, a2, a3));
		return a3;

	case R_DEFINE2:
		generate_children_code(code, scope, tree);
		break;

	case R_DEFINE3:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		LIST_ADD(code->instructions,
				 create_instruction(I_ASSIGN,
									symbol->address,
									populate_code(code, scope, tree->children[0]),
									NULL));
		break;

	case R_DEFINE4:
		populate_code(code, scope, tree->children[1]);
		break;

	case R_FIELD:
		populate_code(code, scope, tree->children[3]);
		break;

	case R_CLASS1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.class.scope;
		LIST_ADD(code->instructions, create_label(I_LABEL, symbol->start_label));
		generate_children_code(code, scope, tree);
		break;

	case R_METHOD1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.method.scope;
		offset = scope->symbols->count;
		if (strcmp(tree->token->text, "main") == 0)
		{
			has_main = TRUE;
			LIST_ADD(code->instructions, create_label(I_LABEL, "start"));
		}
		LIST_ADD(code->instructions, create_label(I_LABEL, symbol->start_label));
		populate_code(code, scope, tree->children[4]);
		LIST_ADD(code->instructions, create_instruction(I_RETURN, NULL, NULL, NULL));
		break;

	case R_IF1:
		a1 = populate_code(code, scope, tree->children[0]);
		a2 = create_label_address(message("%d", i++));
		i1 = create_instruction(I_JUMP_FALSE, a1, a2, NULL);
		LIST_ADD(code->instructions, i1);
		populate_code(code, scope, tree->children[1]);
		LIST_ADD(code->instructions, create_label(I_LABEL, a2->label));
		break;

	case R_FOR:
		populate_code(code, scope, tree->children[0]);
		i1 = create_label(I_LABEL, message("loop_body%d", i++));
		i2 = create_label(I_LABEL, message("loop_if%d", i++));
		LIST_ADD(code->instructions,
				 create_instruction(I_JUMP,
									create_label_address(i2->extra.label.name),
									NULL,
									NULL));
		LIST_ADD(code->instructions, i1);
		populate_code(code, scope, tree->children[3]);
		populate_code(code, scope, tree->children[2]);
		LIST_ADD(code->instructions, i2);
		populate_code(code, scope, tree->children[1]);
		LIST_ADD(code->instructions,
				 create_instruction(I_JUMP_TRUE,
									populate_code(code, scope, tree->children[1]),
									create_label_address(i1->extra.label.name),
									NULL));
		break;

	case R_CASE_GROUP:
	case R_STATEMENT_GROUP:
	case R_CLASS_GROUP:
		generate_children_code(code, scope, tree);
		break;

	case R_RETURN2:
		a1 = populate_code(code, scope, tree->children[0]);
	case R_RETURN1:
		LIST_ADD(code->instructions, create_instruction(I_RETURN, a1, NULL, NULL));
		break;

	default:
		error(SEMATIC_ERROR, message("Undefined code generation rule %d", tree->rule));
		break;
	}

	return NULL;
}

ICode *generate_code(SymbolTable *scope, Tree *tree)
{
	i = 0;

	ICode *code = alloc(sizeof(ICode));
	code->strings = NULL;
	code->instructions = NULL;

	LIST_ADD(code->instructions, create_instruction(I_JUMP, create_label_address("start"), NULL, NULL));

	populate_code(code, scope, tree);

	if (!has_main)
		error_at(tree->token, SEMATIC_ERROR, "Program does not contain a main method");

#ifdef DEBUG
	printf("Intermediate code generation done\n");
#endif

	return code;
}

void print_code(ICode *code)
{
	printf(".string\n");

	foreach_list(instr, code->strings)
		printf("    %s\\0\n", (char *)instr->value);

	printf(".code\n");

	foreach_list(instr, code->instructions)
		print_instruction((Instruction *)instr->value);
}