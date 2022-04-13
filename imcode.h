/*
	Douglas Newquist
*/

#ifndef _IMCODE
#define _IMCODE

#define ICode struct icode

#include "list.h"
#include "symboltable.h"
#include "tree.h"

ICode
{
	List *strings;
	List *globals;
	List *instructions;
};

ICode *generate_code(SymbolTable *scope, Tree *tree);

void print_code(ICode *code);
#endif
