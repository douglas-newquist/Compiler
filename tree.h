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

/**
 * @brief Boxes the given token into a tree node
 */
Tree *tree_token(Token *token);

/**
 * @brief Prints the given token tree
 *
 * @param tree Tree to print
 * @param indent_level How many indents to start with
 * @param indent What to print for each indent level
 */
void print_tree(Tree *tree, int indent_level, char *indent);

void print_trees(Tree *tree);

/**
 * @brief Creates a tree with n children
 *
 * @param message Name to give this tree
 * @param rule Syntax rule used for this tree
 * @param argc Number of trees being passed
 * @param ... Trees to be added as children
 */
Tree *tree(char *message, int rule, Token *token, int argc, ...);

/**
 * @brief Frees all loaded trees
 */
void free_trees();

#endif
