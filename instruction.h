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
	I_ASSIGN,
	I_CALL,
	I_DATA,
	I_DIV,
	I_JUMP_IF,
	I_JUMP,
	I_LABEL,
	I_LOAD,
	I_MULT,
	I_PARAM,
	I_REGION,
	I_RETURN,
	I_STORE,
	I_SUB,
};

Instruction
{
	int code;
	Address *a1, *a2, *a3;

	union extra
	{
		struct label
		{
			char *name;
		} label;
		struct branch
		{
			char * true;
			char * false;
		} branch;
	} extra;
};

Instruction *create_instruction(int instruction, Address *a1, Address *a2, Address *a3);

Instruction *create_label(int instruction, char *name);

void print_instruction(Instruction *instr);
#endif
