/*
	Douglas Newquist
*/

#ifndef _ADDRESS
#define _ADDRESS

#include <stdlib.h>

#define Address struct address

#define NO_JUMP NULL

#include "symboltable.h"
#include "tree.h"

enum regions
{
	RE_GLOBAL = 4000,
	RE_CONST,
	RE_IMMED,
	RE_LOCAL,
	RE_PARAM,
	RE_STRINGS,
	RE_STRUCT,
};

Address
{
	int region, offset;
	char *label;
};

/**
 * @brief Create an address in the given region and offset
 */
Address *create_address(int region, int offset);

#define Const(x) create_address(RE_IMMED, x)

Address *create_label_address(char *name);

char *address_name(Address *address);
#endif
