/*
	Douglas Newquist
*/

#include "instruction.h"
#include "mmemory.h"

Instruction *create_instruction(int instruction)
{
	Instruction *instr = alloc(sizeof(Instruction));
	instr->code = instruction;
	instr->a1 = NULL;
	instr->a2 = NULL;
	instr->a3 = NULL;

	return instr;
}
