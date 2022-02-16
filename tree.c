
#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "tree.h"
#include "main.h"

void free_node(Node *node, int recursively)
{
	if (node == NULL)
		return;

	if (recursively)
		for (int i = 0; i < node->count; i++)
			free_node(node->children[i], recursively);

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
	Node *n = nnode("Sequence", SEQUENCE, NULL, 2);

	n->children[0] = s1;
	n->children[1] = s2;

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

Node *trinary_op(char *message, int op, Node *v1, Node *v2, Node *v3)
{
	printf("Creating trinary op %d\t%d\t%d\n", v1->op, v2->op, v3->op);
	Node *n = nnode(message, op, NULL, 3);

	n->children[0] = v1;
	n->children[1] = v2;
	n->children[2] = v3;

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
