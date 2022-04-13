/*
	Douglas Newquist
*/

#ifndef _TREE
#define _TREE

#define Tree struct tree

#include "token.h"
#include "list.h"

List *trees;

Tree
{
	int id;
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

/**
 * @brief Prints the given tree in .dot file format
 *
 * @param tree Tree to print
 * @param indent_level Used to detect if a tree is a subtree
 */
void print_dot_tree(Tree *tree, int indent_level);

/**
 * @brief Creates a tree with n children
 *
 * @param message Name to give this tree
 * @param rule Syntax rule used for this tree
 * @param argc Number of trees being passed
 * @param ... Trees to be added as children
 */
Tree *tree(char *message, int rule, Tree *leaf, int argc, ...);

/**
 * @brief Combines multple trees together, automatically merges group trees
 */
Tree *group(char *message, int rule, Tree *t1, Tree *t2);

/**
 * @brief Finds the closest token from the root of the given tree
 */
Token *find_nearest(Tree *tree);
#endif
