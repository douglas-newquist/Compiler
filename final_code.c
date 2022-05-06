/*
	Douglas Newquist
*/

#include <strings.h>
#include <stdlib.h>
#include "errors.h"
#include "final_code.h"
#include "instruction.h"
#include "main.h"
#include "rules.h"
#include "token.h"

#define rbp "%rbp"
#define rsp "%rsp"
#define edx "%edx"
#define eax "%eax"
#define al "%al"

char *get_value(Address *a)
{
	if (a == NULL)
		return "";

	switch (a->region)
	{
	case RE_GLOBAL:
		return message("G%d", a->offset / BYTE_SIZE);

	case RE_IMMED:
		return message("$%d", a->offset);

	case RE_LOCAL:
		return message("-%d(%s)", a->offset, rbp);

	case RE_PARAM:
		return message("%d(%s)", a->offset, rbp);

	case RE_STRINGS:
		return message("LC%d", a->offset / BYTE_SIZE);
	}

	return message("TODO(%d)", a->region);
}

void simple_op3(char *name, Instruction *instr, FILE *file)
{
	fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
	fprintf(file, "\t%s\t%s, %s\n", name, get_value(instr->a2), eax);
	fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
}

void write_strings(List *strings, FILE *file)
{
	int i = 0;
	foreach_list(element, strings)
	{
		Token *string = (Token *)element->value;
		fprintf(file, ".LC%d:\n", i);
		fprintf(file, ".string %s\n", string->text);
		fprintf(file, ".align %d\n", BYTE_SIZE);
		i++;
	}
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
			fprintf(file, "\tcall\t%s, %s, %s\n",
					instr->a1->label,
					get_value(instr->a2),
					get_value(instr->a3));
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
				fprintf(file, "\tmov\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tret\n");
			break;

		case R_OP1_DECREMENT:
			fprintf(file, "\tsubq\t1, %s\n", get_value(instr->a1));
			break;

		case R_OP1_INCREMENT:
			fprintf(file, "\taddq\t1, %s\n", get_value(instr->a1));
			break;

		case R_OP1_NEGATE:
			fprintf(file, "\tnegq\t%s\n", get_value(instr->a1));
			break;

		case R_OP2_ADD:
		case R_OP2_OR:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\taddq\t%s, %s\n", get_value(instr->a2), eax);
			fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
			break;

		case R_OP2_DIV:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tidivq\t%s\n", get_value(instr->a2));
			fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
			break;

		case R_OP2_EQUALS:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tcmpq\t%s, %s\n", get_value(instr->a2), eax);
			fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
			break;

		case R_OP2_GREATER_EQUAL:
			break;

		case R_OP2_GREATER:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tcmpq\t%s, %s\n", get_value(instr->a2), eax);
			fprintf(file, "\tsetg\t%s\n", al);
			fprintf(file, "\tmovzbl\t%s, %s\n", al, eax);
			fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
			break;

		case R_OP2_MOD:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tidivq\t%s\n", get_value(instr->a2));
			fprintf(file, "\tmovq\t%s, %s\n", edx, get_value(instr->a3));
			break;

		case R_OP2_MULT:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tmulq\t%s\n", get_value(instr->a2));
			fprintf(file, "\tmovq\t%s, %s\n", edx, get_value(instr->a3));
			break;

		case R_OP2_SUB:
			fprintf(file, "\tmovq\t%s, %s\n", get_value(instr->a1), eax);
			fprintf(file, "\tsubq\t%s, %s\n", get_value(instr->a2), eax);
			fprintf(file, "\tmovq\t%s, %s\n", eax, get_value(instr->a3));
			break;

		default:
			fprintf(file, "\t%s not implemented\n", instr_name(instr->code));
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
