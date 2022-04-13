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
// Stores the label of the next continue point
char *next_continue = NULL;
// Stores the label of the next break point
char *next_break = NULL;
char *next_if = NULL;
int code_region = RE_LOCAL;

void add_instr(ICode *code, Instruction *instruction)
{
	switch (code_region)
	{
	case RE_GLOBAL:
		LIST_ADD(code->globals, instruction);
		break;

	default:
		LIST_ADD(code->instructions, instruction);
		break;
	}
}

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
	// Previous continue/break
	char *prev_contine = NULL, *prev_break = NULL;
	int j = 0;

	set_pos(find_nearest(tree));

	switch (tree->rule)
	{
	case R_EMPTY:
		return NULL;

	case CONTINUE:
		if (next_continue == NULL)
			error(SEMATIC_ERROR, "Cannot continue here");
		add_instr(code, create_instruction(I_JUMP, create_label_address(next_continue), NULL, NULL));
		return NULL;

	case LITERAL_BOOL:
		return create_address(RE_CONST, tree->token->bval);

	case LITERAL_CHAR:
		return create_address(RE_CONST, tree->token->cval);

	case LITERAL_DOUBLE:
		// TODO
		break;

	case LITERAL_INT:
		return create_address(RE_CONST, tree->token->ival);

	case LITERAL_STRING:
		foreach_list(string, code->strings)
		{
			if (strcmp((char *)string->value, tree->token->sval) == 0)
				return create_address(RE_STRINGS, j * BYTE_SIZE);
			j++;
		}
		LIST_ADD(code->strings, tree->token->sval);
		return create_address(RE_STRINGS, (code->strings->size - 1) * BYTE_SIZE);

	case LITERAL_NULL:
		return create_address(RE_CONST, 0);

	case ID:
	case R_ACCESS1:
	case R_ACCESS2:
	case R_DEFINE1:
		symbol = lookup_name(scope, tree, SCOPE_SYMBOLS);

		if (tree->token->symbol)
		{
			symbol = tree->token->symbol;
			if (symbol->attributes & ATR_VARIABLE)
			{
				i1 = create_instruction(I_ASSIGN, symbol->address, Const(0), NULL);
				add_instr(code, i1);
			}
		}

		return symbol->address;

	case R_ACCESS3:
		// TODO
		break;

	case R_ARG_DEF_GROUP:
		// TODO
		break;

	case R_ARG_GROUP:
		for (j = tree->count - 1; j > -1; j--)
			add_instr(code, create_instruction(I_PARAM,
											   populate_code(code, scope, tree->children[j]),
											   NULL,
											   NULL));
		return NULL;

	case R_ARRAY1:
		break;

	case R_ARRAY2:
		a1 = create_address(RE_LOCAL, (offset++) * BYTE_SIZE);
		a2 = create_address(RE_LOCAL, (offset++) * BYTE_SIZE);
		add_instr(code, create_instruction(R_OP2_MULT,
										   a2,
										   populate_code(code, scope, tree->children[1]),
										   Const(BYTE_SIZE)));
		add_instr(code, create_instruction(I_ALLOC,
										   a1,
										   a2,
										   NULL));
		return a1;

	case R_ASSIGN1:
		a1 = populate_code(code, scope, tree->children[0]);
		a2 = populate_code(code, scope, tree->children[1]);
		add_instr(code, create_instruction(I_ASSIGN, a1, a2, NULL));
		return a1;

	case R_ASSIGN2:
		// TODO
		break;

	case R_BREAK1:
	case R_BREAK2:
		if (next_break == NULL)
			error(SEMATIC_ERROR, "Cannot break here");
		add_instr(code, create_instruction(I_JUMP,
										   create_label_address(next_break),
										   NULL,
										   NULL));
		return NULL;

	case R_CALL1:
		symbol = lookup_name(scope, tree->children[0], SCOPE_SYMBOLS);
		switch (tree->children[1]->rule)
		{
		case R_EMPTY:
			break;

		case R_ARG_GROUP:
			populate_code(code, scope, tree->children[1]);
			break;

		default:
			add_instr(code, create_instruction(I_PARAM,
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
		add_instr(code, create_instruction(I_CALL, a1, a2, a3));
		return a3;

	case R_CALL2:
		// TODO
		break;

	case R_CASE_GROUP:
		generate_children_code(code, scope, tree);
		return NULL;

	case R_CASE:
		// TODO
		break;

	case R_CLASS_GROUP:
		code_region = RE_GLOBAL;
		for (j = 0; j < tree->count; j++)
		{
			switch (tree->children[j]->rule)
			{
			case R_FIELD:
				populate_code(code, scope, tree->children[j]);
				break;
			}
		}

		code_region = RE_LOCAL;
		for (j = 0; j < tree->count; j++)
		{
			switch (tree->children[j]->rule)
			{
			case R_METHOD1:
			case R_METHOD2:
				populate_code(code, scope, tree->children[j]);
				break;
			}
		}
		return NULL;

	case R_CLASS1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.class.scope;
		return populate_code(code, scope, tree->children[1]);

	case R_DEFINE2:
		generate_children_code(code, scope, tree);
		return NULL;

	case R_DEFINE3:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		i1 = create_instruction(I_ASSIGN,
								symbol->address,
								populate_code(code, scope, tree->children[0]),
								NULL);
		add_instr(code, i1);
		return symbol->address;

	case R_DEFINE4:
		populate_code(code, scope, tree->children[1]);
		return NULL;

	case R_EXP_GROUP:
		// TODO
		break;

	case R_FIELD:
		return populate_code(code, scope, tree->children[3]);

	case R_FOR:
		prev_break = next_break;
		prev_contine = next_continue;
		next_continue = message("continue%d", i++);
		next_break = message("break%d", i++);
		populate_code(code, scope, tree->children[0]);
		i1 = create_label(I_LABEL, message("loop_body%d", i++));
		i2 = create_label(I_LABEL, message("loop_if%d", i - 1));
		add_instr(code,
				  create_instruction(I_JUMP,
									 create_label_address(i2->extra.label.name),
									 NULL,
									 NULL));
		add_instr(code, i1);
		populate_code(code, scope, tree->children[3]);
		add_instr(code, create_label(I_LABEL, next_continue));
		populate_code(code, scope, tree->children[2]);
		add_instr(code, i2);
		populate_code(code, scope, tree->children[1]);
		add_instr(code,
				  create_instruction(I_JUMP_TRUE,
									 create_label_address(i1->extra.label.name),
									 populate_code(code, scope, tree->children[1]),
									 NULL));
		add_instr(code, create_label(I_LABEL, next_break));
		next_break = prev_break;
		next_continue = prev_contine;
		return NULL;

	case R_IF1:
		i1 = create_label(I_LABEL, message("%d", i++));
		add_instr(code,
				  create_instruction(I_JUMP_FALSE,
									 create_label_address(i1->extra.label.name),
									 populate_code(code, scope, tree->children[0]),
									 NULL));
		populate_code(code, scope, tree->children[1]);
		if (next_if)
			add_instr(code,
					  create_instruction(I_JUMP, create_label_address(next_if), NULL, NULL));
		add_instr(code, i1);
		return NULL;

	case R_IF2:
	case R_IF3:
		if (next_if)
			generate_children_code(code, scope, tree);
		else
		{
			next_if = message("%d", i++);
			generate_children_code(code, scope, tree);
			add_instr(code, create_label(I_LABEL, next_if));
		}
		return NULL;

	case R_METHOD1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.method.scope;
		offset = scope->symbols->count;
		if (strcmp(tree->token->text, "main") == 0)
		{
			has_main = TRUE;
			add_instr(code, create_label(I_LABEL, "start"));
		}
		add_instr(code, create_label(I_LABEL, symbol->start_label));
		populate_code(code, scope, tree->children[4]);
		add_instr(code, create_instruction(I_RETURN, NULL, NULL, NULL));
		return NULL;

	case R_METHOD2:
		// TODO
		break;

	case R_OP1_DECREMENT:
	case R_OP1_INCREMENT:
		a1 = populate_code(code, scope, tree->children[0]);
		add_instr(code, create_instruction(tree->rule, a1, NULL, NULL));
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
		add_instr(code, create_instruction(tree->rule, a1, a2, a3));
		return a1;

	case R_RETURN2:
		a1 = populate_code(code, scope, tree->children[0]);
	case R_RETURN1:
		add_instr(code, create_instruction(I_RETURN, a1, NULL, NULL));
		return a1;

	case R_STATEMENT_GROUP:
		generate_children_code(code, scope, tree);
		return NULL;

	case R_SWITCH:
		prev_break = next_break;
		next_break = message("break%d", i++);
		// TODO
		add_instr(code, create_label(I_LABEL, next_break));
		next_break = prev_break;
		return NULL;

	case R_VAR_GROUP:
		// TODO
		break;

	case R_WHILE:
		prev_break = next_break;
		prev_contine = next_continue;
		i1 = create_label(I_LABEL, message("while_body%d", i++));
		next_continue = message("while_if%d", i - 1);
		i2 = create_label(I_LABEL, next_continue);
		next_break = message("break%d", i++);
		add_instr(code,
				  create_instruction(I_JUMP,
									 create_label_address(i2->extra.label.name),
									 NULL, NULL));
		add_instr(code, i1);
		populate_code(code, scope, tree->children[1]);
		add_instr(code, i2);
		add_instr(code,
				  create_instruction(I_JUMP_TRUE,
									 create_label_address(i1->extra.label.name),
									 populate_code(code, scope, tree->children[0]),
									 NULL));
		add_instr(code, create_label(I_LABEL, next_break));
		next_break = prev_break;
		next_continue = prev_contine;
		return NULL;
	}

	error(SEMATIC_ERROR, message("Undefined code generation rule %d", tree->rule));
	return NULL;
}

ICode *generate_code(SymbolTable *scope, Tree *tree)
{
	i = 0;

	ICode *code = alloc(sizeof(ICode));
	code->strings = NULL;
	code->globals = NULL;
	code->instructions = NULL;

	add_instr(code, create_instruction(I_JUMP, create_label_address("start"), NULL, NULL));

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

	foreach_list(instr, code->globals)
		print_instruction((Instruction *)instr->value);

	foreach_list(instr, code->instructions)
		print_instruction((Instruction *)instr->value);
}
