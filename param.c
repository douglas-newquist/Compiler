
#include <stdlib.h>
#include "mmemory.h"
#include "param.h"

Param *create_param(char *name, Type *type)
{
	Param *param = alloc(sizeof(Param));
	param->name = name;
	param->type = type;

	return param;
}

Param *simple_param(char *name, int type)
{
	return create_param(name, create_type(type));
}

void alloc_params(Type *type, int count)
{
	type->info.method.count = count;
	type->info.method.params = alloc(sizeof(Param *) * count);
}
