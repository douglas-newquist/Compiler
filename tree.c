/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "tree.h"
#include "token.h"
#include "rules.h"

/**
 * @brief Create a new tree object
 *
 * @param message Name to give this tree
 * @param rule Rule that was used to construct this tree
 * @param child_count Number of child trees
 * @param token
 */
Tree *create_tree(char *message, int rule, int child_count, Token *leaf)
{
	Tree *tree = (Tree *)malloc(sizeof(Tree));

	// Store tree values
	tree->rule = rule;
	tree->name = message;
	tree->count = child_count;
	tree->token = leaf;

	// Create array for children if needed
	if (child_count > 0)
		tree->children = (Tree **)malloc(sizeof(Tree *) * child_count);
	else
		tree->children = NULL;

	// Add tree to list
	trees = add(trees, tree);

	return tree;
}

Tree *tree_token(Token *token)
{
	return create_tree("Token", token->category, 0, token);
}

/**
 * @brief Frees the given tree
 */
void free_tree(Tree *tree)
{
	if (tree == NULL)
		return;

	free(tree->children);
	free(tree);
}

/**
 * @brief Wrapper for free_tree for linked list
 */
void free_list_tree(void *tree)
{
	free_tree((Tree *)tree);
}

void free_trees()
{
	trees = free_list(trees, free_list_tree);
}

/**
 * @brief Prints the given token tree
 *
 * @param tree Tree to print
 * @param indent_level How many indents to start with
 * @param indent What to print for each indent level
 */
void print_tree(Tree *tree, int indent_level, char *indent)
{
	// Print indent characters
	for (int i = 0; i < indent_level; i++)
		printf("%s", indent);

	if (tree == NULL)
	{
		printf("(nil)\n");
		return;
	}

	// Print basic information on the node
	printf("%s(R:%d, C:%d)",
		   tree->name,
		   tree->rule,
		   tree->count);

	// If a token is present print its text
	if (tree->token)
	{
		printf(" %s ", tree->token->text);
		print_token_value(tree->token);
	}

	printf("\n");

	// Print children
	for (int i = 0; i < tree->count; i++)
		print_tree(tree->children[i], indent_level + 1, indent);
}

void print_trees(Tree *tree)
{
	printf("--------------------------------------------------------------\n");
	print_tree(tree, 0, "| ");
	printf("--------------------------------------------------------------\n");
}

/**
 * @brief Creates a tree with n children
 *
 * @param message Name to give this tree
 * @param rule Syntax rule used for this tree
 * @param argc Number of trees being passed
 * @param ... Trees to be added as children
 */
Tree *tree(char *message, int rule, Tree *leaf, int argc, ...)
{
	Tree *tree;

	if (leaf)
		tree = create_tree(message, rule, argc, leaf->token);
	else
		tree = create_tree(message, rule, argc, NULL);

	// Begin array of input trees
	va_list args;
	va_start(args, argc);

	// Store children
	for (int i = 0; i < argc; i++)
		tree->children[i] = va_arg(args, Tree *);

	// End array of trees
	va_end(args);

	return tree;
}

/**
 * @brief Combines multple trees together, automatically merges group trees
 */
Tree *group(Tree *t1, Tree *t2)
{
	// Check if either tree is a group
	if (t1->rule != R_GROUP && t2->rule != R_GROUP)
		// Neither is a group make a new one
		return tree("Group", R_GROUP, NULL, 2, t1, t2);

	// New group size
	int size = 2;

	// If t1 is a group add its size
	if (t1->rule == R_GROUP)
		size += t1->count - 1;

	// If t2 is a group add its size
	if (t2->rule == R_GROUP)
		size += t2->count - 1;

	Tree *t = create_tree("Group", R_GROUP, size, NULL);
	int offset = 1;

	// Add t1 to the group
	if (t1->rule == R_GROUP)
	{
		// Copy children from t1 to the new group
		for (int i = 0; i < t1->count; i++)
			t->children[i] = t1->children[i];

		offset = t1->count;
	}
	else
		// t1 was not a group
		t->children[0] = t1;

	// Add t2 to the group
	if (t2->rule == R_GROUP)
	{
		// Copy children from t2 to the new group
		for (int i = 0; i < t2->count; i++)
			t->children[i + offset] = t2->children[i];
	}
	else
		// t2 was not a group
		t->children[offset] = t2;

	return t;
}
