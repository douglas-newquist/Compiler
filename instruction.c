/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "main.h"
#include "mmemory.h"
#include "rules.h"

Instruction *create_instruction(int instruction, Address *a1, Address *a2, Address *a3)
{
	Instruction *instr = alloc(sizeof(Instruction));
	instr->code = instruction;
	instr->a1 = a1;
	instr->a2 = a2;
	instr->a3 = a3;

	return instr;
}

Instruction *create_label(int instruction, char *name)
{
	Instruction *instr = create_instruction(instruction, NULL, NULL, NULL);
	instr->extra.label.name = name;
	return instr;
}

char *instr_name(int instruction)
{
	switch (instruction)
	{
	case I_ASSIGN:
		return "ASSIGN";
	case I_CALL:
		return "CALL";
	case I_DATA:
		return "DATA";
	case I_JUMP_IF:
		return "BIF";
	case I_JUMP:
		return "JUMP";
	case I_JUMP_FALSE:
		return "JUMPF";
	case I_JUMP_TRUE:
		return "JUMPT";
	case I_PARAM:
		return "PARAM";
	case I_RETURN:
		return "RETURN";

	case R_OP1_NEGATE:
		return "NEGATE";
	case R_OP1_NOT:
		return "LNOT";
	case R_OP2_ADD:
		return "ADD";
	case R_OP2_AND:
		return "LAND";
	case R_OP2_DIV:
		return "DIV";
	case R_OP2_EQUALS:
		return "EQ";
	case R_OP2_GREATER_EQUAL:
		return "GEQ";
	case R_OP2_GREATER:
		return "GT";
	case R_OP2_LESS_EQUAL:
		return "LEQ";
	case R_OP2_LESS:
		return "LT";
	case R_OP2_MOD:
		return "MOD";
	case R_OP2_MULT:
		return "MULT";
	case R_OP2_NOT_EQUAL:
		return "NEQ";
	case R_OP2_OR:
		return "LOR";
	case R_OP2_SUB:
		return "SUB";

	default:
		return message("%d", instruction);
	}
}

void print_instruction(Instruction *instr)
{
	switch (instr->code)
	{
	case I_LABEL:
	case I_DATA:
	case I_REGION:
		printf("%s\n", instr->extra.label.name);
		break;

	default:
#ifdef DEBUG
		printf("    %s %s, %s, %s\n",
			   instr_name(instr->code),
			   address_name(instr->a1),
			   address_name(instr->a2),
			   address_name(instr->a3));
#else
		printf("    %s %s %s %s\n",
			   instr_name(instr->code),
			   address_name(instr->a1),
			   address_name(instr->a2),
			   address_name(instr->a3));
#endif
	}
}
