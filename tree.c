/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "tree.h"
#include "token.h"
#include "rules.h"

Tree *create_tree(char *message, int rule, int child_count, Token *token)
{
	Tree *tree = (Tree *)malloc(sizeof(Tree));

	tree->rule = rule;
	tree->name = message;
	tree->count = child_count;
	tree->token = token;

	if (child_count > 0)
		tree->children = (Tree **)malloc(sizeof(Tree *) * child_count);
	else
		tree->children = NULL;

	trees = add(trees, tree);

	return tree;
}

Tree *tree_token(Token *token)
{
	return create_tree(token->text, token->category, 0, token);
}

void free_tree(Tree *tree)
{
	if (tree == NULL)
		return;

	free(tree->children);
	free(tree);
}

void free_list_tree(void *tree)
{
	free_tree((Tree *)tree);
}

void free_trees()
{
	trees = free_list(trees, free_list_tree);
}

void print_tree(Tree *tree, int indent_level, char *indent)
{
	for (int i = 0; i < indent_level; i++)
		printf("%s", indent);

	if (tree == NULL)
	{
		printf("NULL\n");
		return;
	}

	printf("%s(R:%d, C:%d)",
		   tree->name,
		   tree->rule,
		   tree->count);

	if (tree->token)
		printf(" %s on line %d\n", tree->token->text, tree->token->line);
	else
		printf("\n");

	for (int i = 0; i < tree->count; i++)
		print_tree(tree->children[i], indent_level + 1, indent);
}

Tree *tree(char *message, int rule, Token *token, int argc, ...)
{
	Tree *tree = create_tree(message, rule, argc, token);

	// Begin array of input trees
	va_list args;
	va_start(args, argc);

	for (int i = 0; i < argc; i++)
		tree->children[i] = va_arg(args, Tree *);

	// End array of trees
	va_end(args);

	return tree;
}
