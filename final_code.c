/*
	Douglas Newquist
*/

#include <strings.h>
#include <stdlib.h>
#include "errors.h"
#include "final_code.h"
#include "instruction.h"
#include "rules.h"
#include "main.h"

char *rbp = "%rbp";
char *rsp = "%rsp";
char *edx = "%edx";
char *eax = "%eax";

char *get_value(Address *a)
{
	switch (a->region)
	{
	case RE_GLOBAL:
		return message("G%d", a->offset / BYTE_SIZE);

	case RE_IMMED:
		return message("%d", a->offset);

	case RE_LOCAL:
		return message("-%d(%s)", a->offset, rbp);
	}

	return message("TODO(%d)", a->region);
}

void write_strings(List *strings, FILE *file)
{
}

void write_data(List *data, FILE *file)
{
}

void write_instructions(List *instructions, FILE *file)
{
	foreach_list(element, instructions)
	{
		Instruction *instr = (Instruction *)element->value;

		switch (instr->code)
		{
		case I_ASSIGN:
			fprintf(file, "\tmovq\t%s, %s\n",
					get_value(instr->a1),
					get_value(instr->a2));
			break;

		case I_MAIN:
			fprintf(file, "\tpushq\t0\n");
		case I_CALL:
			fprintf(file, "\tpushq\t%s\n", rbp);
			fprintf(file, "\tmovq\t%s, %s\n", rsp, rbp);
			fprintf(file, "\tcall\t%s\n", instr->a1->label);
			fprintf(file, "\tpopq\t%s\n", rbp);
			break;

		case I_EXIT:
			fprintf(file, "\tret\n");
			break;

		case I_JUMP:
			fprintf(file, "\tjmp\t%s\n", instr->a1->label);
			break;

		case I_LABEL:
			fprintf(file, "%s:\n", instr->extra.label.name);
			break;

		case I_PARAM:
			fprintf(file, "\tpushq\t%s\n", get_value(instr->a1));
			break;

		case I_RETURN:
			if (instr->a1)
				fprintf(file, "\tmov\t%s, %s\n", edx, get_value(instr->a1));
			fprintf(file, "\tret\n");
			break;

		case R_OP1_DECREMENT:
			fprintf(file, "\tsubq\t%s, 1\n", get_value(instr->a1));
			break;

		case R_OP1_INCREMENT:
			fprintf(file, "\taddq\t%s, 1\n", get_value(instr->a1));
			break;

		case R_OP2_EQUALS:
			fprintf(file, "\tcmpq\t%s, %s\n",
					get_value(instr->a1),
					get_value(instr->a2));
			break;

		default:
#ifdef DEBUG
			fprintf(file, "\t%s not implemented\n", instr_name(instr->code));
#else
			error(SEMATIC_ERROR, message("Instruction code %d not implemented", instr->code));
#endif
			break;
		}
	}
}

void write_final_code(ICode *code, char *filename)
{
	if (code == NULL)
		return;

	FILE *file = fopen(filename, "w");

	write_strings(code->strings, file);
	write_data(code->data, file);
	write_instructions(code->globals, file);
	write_instructions(code->instructions, file);

	fclose(file);
}
