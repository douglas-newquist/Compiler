/*
	Douglas Newquist
*/

#ifndef _ADDRESS
#define _ADDRESS
#define Address struct address

enum regions
{
	RE_GLOBAL = 4000,
	RE_STRUCT,
	RE_PARAM,
	RE_LOCAL,
	RE_CONST,
	RE_IMMED
};

Address
{
	int region, offset;
};

Address *create_address(int region, int offset);

#endif
