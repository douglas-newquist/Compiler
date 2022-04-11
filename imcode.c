/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include "id.h"
#include "imcode.h"
#include "instruction.h"
#include "rules.h"

List *generate_children_code(SymbolTable *scope, Tree *tree)
{
	List *code = NULL;

	for (int i = 0; i < tree->count; i++)
		code = list_merge(code,
						  generate_code(scope, tree->children[i]));

	return code;
}

List *generate_code(SymbolTable *scope, Tree *tree)
{
	List *code = NULL;

	switch (tree->rule)
	{
	case R_CLASS1:
		break;

	default:
		printf("Undefined code generation rule: %d\n", tree->rule);
		return generate_children_code(scope, tree);
	}

	return code;
}

void print_code(List *code)
{
	foreach_list(instr, code)
	{
		Instruction *i = (Instruction *)instr;
		printf("%d\n", i->code);
	}
}
