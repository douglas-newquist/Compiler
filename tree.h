/*
	Douglas Newquist
*/

#include "token.h"
#include "list.h"

#ifndef Tree
#define Tree struct tree
#define Trees List

Trees *trees;

Tree
{
	int rule;
	char *name;
	int count;
	Token *token;
	Tree **children;
};

Tree *tree_token(Token *token);

void print_tree(Tree *tree, int indent_level, char *indent);

Tree *tree(char *message, int rule, Token *token, int argc, ...);

void free_trees();

#endif
