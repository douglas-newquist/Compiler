/*
	Douglas Newquist
*/

#ifndef _INSTRUCTION
#define _INSTRUCTION

#define Instruction struct instruction
#define BYTE_SIZE 8

#include <stdio.h>
#include "address.h"

enum
{
	I_ALLOC = 5000,
	I_ASSIGN,
	I_CALL,
	I_DATA,
	I_EXIT,
	I_JUMP_FALSE,
	I_JUMP_IF,
	I_JUMP_TRUE,
	I_JUMP,
	I_LABEL,
	I_LOAD,
	I_MAIN,
	I_PARAM,
	I_REGION,
	I_RETURN,
	I_STORE,
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

char *instr_name(int instruction);

Instruction *create_label(int instruction, char *name);

void print_instruction(Instruction *instr, FILE *file);
#endif
