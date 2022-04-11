/*
	Douglas Newquist
*/

#ifndef _IMCODE
#define _IMCODE

#include "list.h"
#include "symboltable.h"
#include "tree.h"

List *generate_code(SymbolTable *scope, Tree *tree);

void print_code(List *code);
#endif
