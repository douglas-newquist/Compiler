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

/**
 * @brief Gets the code for the given flag
 */
int get_flag_code(char *arg)
{
	check_flag(arg, "--none", NONE_FLAG);
	check_flag(arg, "--tokens", TOKENS_FLAG);
	check_flag(arg, "--tree", TREE_FLAG);
	check_flag(arg, "--bison", BISON_FLAG);
	check_flag(arg, "--dot", DOT_FLAG);
	check_flag(arg, "--symbols", SYMBOLS_FLAG);

	return 0;
}

/**
 * @brief Gets the code for the given flag, 0 if not a flag
 */
int flag(char *arg)
{
	int cflag = get_flag_code(arg);

	switch (cflag)
	{
	case NONE_FLAG:
		options = 0;
		break;
	}

	return cflag;
}

int has_flags(int v, int flags)
{
	return (v & flags) == flags;
}
