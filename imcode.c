/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include "address.h"
#include "imcode.h"
#include "instruction.h"
#include "jzero.tab.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"

int i = 0;

void populate_code(ICode *code, SymbolTable *scope, Tree *tree);

void generate_children_code(ICode *code, SymbolTable *scope, Tree *tree)
{
	if (tree == NULL)
		return;

	for (int i = 0; i < tree->count; i++)
		populate_code(code, scope, tree->children[i]);
}

Address *parse_address(ICode *code, SymbolTable *scope, Tree *tree)
{
	Symbol *symbol;
	switch (tree->rule)
	{
	case ID:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		return symbol->address;

	case LITERAL_INT:
		return create_address(RE_CONST, tree->token->ival);

	default:
		return NULL;
	}
}

void populate_code(ICode *code, SymbolTable *scope, Tree *tree)
{
	Symbol *symbol = NULL;
	Instruction *i1, *i2;
	Address *a1 = NULL, *a2 = NULL, *a3 = NULL;

	switch (tree->rule)
	{
	case ID:
	case PUBLIC:
	case R_EMPTY:
	case STATIC:
		break;

	case R_ASSIGN1:
		a3 = parse_address(code, scope, tree->children[1]);
		LIST_ADD(code->instructions, create_instruction(I_ASSIGN, a1, a2, a3));
		break;

	case R_CALL1:
		LIST_ADD(code->instructions, create_instruction(I_CALL, a1, a2, a3));
		break;

	case R_DEFINE4:
		symbol = lookup(scope, tree->children[1]->token->text, SCOPE_SYMBOLS);
		a1 = symbol->address;
		a2 = parse_address(code, scope, tree->children[1]);
		LIST_ADD(code->instructions, create_instruction(I_ASSIGN, a1, a2, a3));
		break;

	case R_FIELD:
		symbol = lookup(scope, tree->children[3]->token->text, SCOPE_SYMBOLS);
		a1 = symbol->address;
		if (tree->children[3])
			LIST_ADD(code->instructions, create_instruction(I_ASSIGN, a1, a2, a3));
		break;

	case R_CLASS1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.class.scope;
		symbol->start_label = message("%s%d:", tree->token->text, i++);
		LIST_ADD(code->instructions, create_label(I_LABEL, symbol->start_label));
		generate_children_code(code, scope, tree);
		break;

	case R_METHOD1:
		symbol = lookup(scope, tree->token->text, SCOPE_SYMBOLS);
		scope = symbol->type->info.method.scope;
		symbol->start_label = message("%s%d:", tree->token->text, i++);
		symbol->end_label = message("%s%d:", tree->token->text, i++);
		LIST_ADD(code->instructions, create_label(I_LABEL, symbol->start_label));
		populate_code(code, scope, tree->children[4]);
		LIST_ADD(code->instructions, create_label(I_LABEL, symbol->end_label));
		LIST_ADD(code->instructions, create_instruction(I_RETURN, NULL, NULL, NULL));
		break;

	case R_IF1:
		// TODO condition
		i1 = create_instruction(I_JUMP_IF, NULL, NULL, NULL);
		LIST_ADD(code->instructions, i1);
		i1->extra.branch.true = NO_JUMP;
		i1->extra.branch.false = message("%d:", i++);
		populate_code(code, scope, tree->children[1]);
		LIST_ADD(code->instructions, create_label(I_LABEL, i1->extra.branch.false));
		break;

	case R_FOR:
		populate_code(code, scope, tree->children[0]);
		populate_code(code, scope, tree->children[3]);
		break;

	case R_CASE_GROUP:
	case R_CLASS_GROUP:
	case R_STATEMENT_GROUP:
		generate_children_code(code, scope, tree);
		break;

	default:
		error(SEMATIC_ERROR, message("Undefined code generation rule %d", tree->rule));
		break;
	}
}

ICode *generate_code(SymbolTable *scope, Tree *tree)
{
	i = 0;

	ICode *code = alloc(sizeof(ICode));
	code->strings = NULL;
	code->instructions = NULL;

	populate_code(code, scope, tree);

	return code;
}

void print_code(ICode *code)
{
	foreach_list(instr, code->strings)
		print_instruction((Instruction *)instr->value);
	foreach_list(instr, code->instructions)
		print_instruction((Instruction *)instr->value);
}
