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
	RE_CLASS,
	RE_CONST,
	RE_IMMED,
	RE_LOCAL,
	RE_PARAM,
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

Address *create_label_address(char *name);

char *address_name(Address *address);
#endif
