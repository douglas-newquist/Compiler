
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"
#include "main.h"

void free_node(Node *node, int recursively)
{
	printf("Freeing node %s\n", node->message);

	if (node == NULL)
		return;

	if (recursively)
		for (int i = 0; i < node->count; i++)
			free_node(node->children[i], recursively);

	//if (node->token)
	//	free_token(node->token);

	if (node->children)
		free(node->children);

	free(node);
}

Node *nnode(char *message, int op, Token *token, int children)
{
	if (token)
		printf("Creating node for %s\n", token->text);
	Node *n = (Node *)malloc(sizeof(Node));

	n->message = message;
	n->op = op;
	n->token = token;
	n->count = children;

	if (children > 0)
		n->children = (Node **)malloc(sizeof(Node *) * children);
	else
		n->children = NULL;

	return n;
}

Node *token_node()
{
	return nnode("T:", ctoken->category, ctoken, 0);
}

Node *sequence(Node *s1, Node *s2)
{
	Node *n;

	int size = 0;

	// Both nodes are sequences, merge them
	if (s1->op == SEQUENCE && s2->op == SEQUENCE)
	{
		n = nnode("Sequence", SEQUENCE, NULL, s1->count + s2->count);

		for (int i = 0; i < s1->count; i++)
			n->children[i] = s1->children[i];

		for (int i = 0; i < s2->count; i++)
			n->children[i + s1->count] = s2->children[i];

		free_node(s1, FALSE);
		free_node(s2, FALSE);
	}
	// Left node is a sequence, right node is something else
	else if (s1->op == SEQUENCE)
	{
		n = nnode("Sequence", SEQUENCE, NULL, 1 + s1->count);
		for (int i = 0; i < s1->count; i++)
			n->children[i] = s1->children[i];

		n->children[s1->count] = s2;

		free_node(s1, FALSE);
	}
	// Right node is a sequence, left node is something else
	else if (s2->op == SEQUENCE)
	{
		n = nnode("Sequence", SEQUENCE, NULL, 1 + s2->count);
		for (int i = 0; i < s2->count; i++)
			n->children[i] = s2->children[i];

		n->children[s2->count] = s1;

		free_node(s2, FALSE);
	}
	// Neither node is a sequence, make a new sequence
	else
	{
		Node *n = nnode("Sequence", SEQUENCE, NULL, 2);
		n->children[0] = s1;
		n->children[1] = s2;
		return n;
	}

	return n;
}

Node *unary_op(char *message, int op, Node *v1)
{
	printf("Creating unary op %d %s\n", op, v1->token->text);
	Node *n = nnode(message, op, NULL, 1);
	n->children[0] = v1;
	return n;
}

Node *binary_op(char *message, int op, Node *v1, Node *v2)
{
	printf("Creating binary op\n");
	Node *n = nnode(message, op, NULL, 2);

	n->children[0] = v1;
	n->children[1] = v2;

	return n;
}

void print_node(Node *node, int indents)
{
	if (node == NULL)
		return;

	for (int i = 0; i < indents; i++)
		printf("| ");

	printf("%s", node->message);

	if (node->token)
		printf("%s\n", node->token->text);
	else
		printf("\n");

	for (int i = 0; i < node->count; i++)
		print_node(node->children[i], indents + 1);
}
