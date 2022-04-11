/*
	Douglas Newquist
*/

#ifndef _INSTRUCTION
#define _INSTRUCTION

#define Instruction struct instruction

#include "address.h"

enum
{
	I_ADD = 5000,
	I_CALL,
	I_DIV,
	I_JIF,
	I_JUMP,
	I_LOAD,
	I_MULT,
	I_PARAM,
	I_RETURN,
	I_STORE,
	I_SUB,
};

Instruction
{
	int code;
	Address *a1, *a2, *a3;
};

Instruction *create_instruction(int instruction);

#endif
