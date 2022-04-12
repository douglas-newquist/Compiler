/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instruction.h"
#include "main.h"
#include "mmemory.h"

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
	case I_ADD:
		return "ADD";
	case I_ASSIGN:
		return "ASSIGN";
	case I_CALL:
		return "CALL";
	case I_DATA:
		return "DATA";
	case I_DIV:
		return "DIV";
	case I_JUMP_IF:
		return "BIF";
	case I_MULT:
		return "MULT";
	case I_PARAM:
		return "PARAM";
	case I_RETURN:
		return "RETURN";
	case I_SUB:
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
		printf("    %s %s %s %s\n",
			   instr_name(instr->code),
			   address_name(instr->a1),
			   address_name(instr->a2),
			   address_name(instr->a3));
	}
}
