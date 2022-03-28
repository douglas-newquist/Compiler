
#ifndef _PARAM
#define _PARAM

#define Param struct param

#include "type.h"

Param
{
	// The name of this param
	char *name;
	// Param type
	Type *type;
};

/**
 * @brief Create a new param
 *
 * @param name
 * @param type
 */
Param *create_param(char *name, Type *type);

/**
 * @brief Creates a new param with a simple type
 *
 * @param name
 * @param type
 */
Param *simple_param(char *name, int type);

/**
 * @brief Initializes the params array in a type
 *
 * @param type Type to modify
 * @param count The number of params
 */
void alloc_params(Type *type, int count);
#endif
