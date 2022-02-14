
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"
#include "main.h"

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
	printf("%s\t%s\n", s1->message, s2->message);
	Node *n;
	int size = 0;

	if (s1->op == SEQUENCE)
		size += s1->count;

	if (s2->op == SEQUENCE)
		size += s2->count;

	if (size == 0)
		size = 2;

	n = nnode("Sequence", SEQUENCE, NULL, size);

	int offset = 1;

	if (s1->op == SEQUENCE)
	{
		for (int i = 0; i < s1->count; i++)
			n->children[i] = s1->children[i];

		offset = s1->count;
	}
	else
		n->children[0] = s1;

	if (s2->op == SEQUENCE)
	{
		for (int i = 0; i < s2->count; i++)
			n->children[i + offset] = s2->children[i];
	}
	else
		n->children[offset] = s2;

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
