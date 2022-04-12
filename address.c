/*
	Douglas Newquist
*/

#include <string.h>
#include "address.h"
#include "main.h"
#include "mmemory.h"

#define NO_JUMP NULL

Address *create_address(int region, int offset)
{
	Address *address = alloc(sizeof(Address));
	address->offset = offset;
	address->region = region;
	address->label = NULL;

	return address;
}

Address *create_label_address(char *name)
{
	Address *address = create_address(RE_IMMED, 0);
	address->label = alloc(sizeof(char) * strlen(name));
	strcpy(address->label, name);
	return address;
}

char *region_name(int region)
{
	switch (region)
	{
	case RE_GLOBAL:
		return "global";

	case RE_CLASS:
		return "class";

	case RE_CONST:
		return "const";

	case RE_LOCAL:
		return "local";

	default:
		return message("%d", region);
	}
}

char *address_name(Address *address)
{
	if (address == NULL)
		return "";

	if (address->label)
		return address->label;

	return message("%s:%d", region_name(address->region), address->offset);
}
