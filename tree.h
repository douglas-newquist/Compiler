/*
	Douglas Newquist
*/

#include "token.h"
#include "list.h"

#ifndef Tree
#define Tree struct tree
#define Trees List

#define EMPTY_TREE tree("END", 0, NULL, 0)

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

/**
 * @brief Creates a tree with n children
 *
 * @param message Name to give this tree
 * @param rule Syntax rule used for this tree
 * @param argc Number of trees being passed
 * @param ... Trees to be added as children
 */
Tree *tree(char *message, int rule, Token *token, int argc, ...);

void free_trees();

#endif
