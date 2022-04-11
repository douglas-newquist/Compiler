/*
	Douglas Newquist
*/

#include "address.h"
#include "mmemory.h"

Address *create_address(int region, int offset)
{
	Address *address = alloc(sizeof(Address));
	address->region = region;
	address->offset = offset;

	return address;
}
