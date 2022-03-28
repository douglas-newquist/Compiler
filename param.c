
#include <stdlib.h>
#include "errors.h"
#include "mmemory.h"
#include "param.h"

/**
 * @brief Create a new param
 *
 * @param name
 * @param type
 */
Param *create_param(char *name, Type *type)
{
	Param *param = alloc(sizeof(Param));
	param->name = name;
	param->type = type;

	return param;
}

/**
 * @brief Creates a new param with a simple type
 *
 * @param name
 * @param type
 */
Param *simple_param(char *name, int type)
{
	return create_param(name, create_type(type));
}

/**
 * @brief Initializes the params array in a type
 *
 * @param type Type to modify
 * @param count The number of params
 */
void alloc_params(Type *type, int count)
{
	type->info.method.count = count;
	type->info.method.params = alloc(sizeof(Param *) * count);
}
