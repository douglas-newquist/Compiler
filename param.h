
#ifndef _PARAM
#define _PARAM

#define Param struct param

#include "type.h"

Param
{
	char *name;
	Type *type;
};

Param *create_param(char *name, Type *type);
Param *simple_param(char *name, int type);
void alloc_params(Type *type, int count);

#endif
