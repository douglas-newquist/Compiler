/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "jzero.tab.h"
#include "tree.h"
#include "token.h"
#include "rules.h"

int tree_count = 0;

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

	tree_count++;

	// Store tree values
	tree->id = tree_count;
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
	trees = list_add(trees, tree);

	return tree;
}

Tree *tree_token(Token *token)
{
	return create_tree("Token", token->category, 0, token);
}

/**
 * @brief Frees the given tree
 */
void free_tree(void *tree)
{
	Tree *t = (Tree *)tree;
	if (t == NULL)
		return;

	free(t->children);
	free(t);
}

void free_trees()
{
	trees = free_list(trees, free_tree);
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
	if (indent_level == 0)
		printf("-----------------------------------------------------------\n");

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

	if (indent_level == 0)
		printf("-----------------------------------------------------------\n");
}

/**
 * @brief Prints the given tree in .dot file format
 *
 * @param tree Tree to print
 * @param indent_level Used to detect if a tree is a subtree
 */
char *tree_dot_name(Tree *tree)
{
	if (tree)
	{
		if (tree->token)
			switch (tree->token->category)
			{
			case LITERAL_STRING:
				return "<string>";

			default:
				return tree->token->text;
			}
		else
			return tree->name;
	}

	return "(nil)";
}

/**
 * @brief Prints the given tree in .dot file format
 *
 * @param tree Tree to print
 * @param indent_level Used to detect if a tree is a subtree
 */
void print_dot_tree(Tree *tree, int indent_level)
{
	if (indent_level == 0)
		printf("graph {\n");

	for (int i = 0; i < tree->count; i++)
	{
		printf("\"%d %s\" -- \"%d %s\"\n",
			   tree->id,
			   tree_dot_name(tree),
			   tree->children[i]->id,
			   tree_dot_name(tree->children[i]));

		print_dot_tree(tree->children[i], indent_level + 1);
	}

	if (indent_level == 0)
		printf("}\n");
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
Tree *group(char *message, int rule, Tree *t1, Tree *t2)
{
	// Check if either tree is a group
	if (t1->rule != rule && t2->rule != rule)
		// Neither is a group make a new one
		return tree(message, rule, NULL, 2, t1, t2);

	// New group size
	int size = 2;

	// If t1 is a group add its size
	if (t1->rule == rule)
		size += t1->count - 1;

	// If t2 is a group add its size
	if (t2->rule == rule)
		size += t2->count - 1;

	Tree *t = create_tree(message, rule, size, NULL);
	int offset = 1;

	// Add t1 to the group
	if (t1->rule == rule)
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
	if (t2->rule == rule)
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
