/*
	Douglas Newquist
*/

#include <string.h>
#include <stdio.h>
#include "flags.h"

int options = 0;
int flag_count = 0;

#define check_flag(arg, compare, code) \
	if (strcmp(compare, arg) == 0)     \
	{                                  \
		flag_count++;                  \
		options |= code;               \
		return code;                   \
	}

int flag(char *arg)
{
	check_flag(arg, "--tokens", TOKENS_FLAG);
	check_flag(arg, "--tree", TREE_FLAG);
	check_flag(arg, "--bison", BISON_FLAG);

	return 0;
}
