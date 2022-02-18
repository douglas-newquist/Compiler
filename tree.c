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
Tree *create_tree(char *message, int rule, int child_count, Token *token)
{
	Tree *tree = (Tree *)malloc(sizeof(Tree));

	// Store tree values
	tree->rule = rule;
	tree->name = message;
	tree->count = child_count;
	tree->token = token;

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
	return create_tree(token->text, token->category, 0, token);
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

	// St
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
		printf(" %s on line %d\n", tree->token->text, tree->token->line);
	else
		printf("\n");

	// Print children
	for (int i = 0; i < tree->count; i++)
		print_tree(tree->children[i], indent_level + 1, indent);
}

/**
 * @brief Creates a tree with n children
 *
 * @param message Name to give this tree
 * @param rule Syntax rule used for this tree
 * @param argc Number of trees being passed
 * @param ... Trees to be added as children
 */
Tree *tree(char *message, int rule, Token *token, int argc, ...)
{
	Tree *tree = create_tree(message, rule, argc, token);

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
