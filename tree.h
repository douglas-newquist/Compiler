
#ifndef TokenTree

#include "token.h"
#include "operations.h"

#define TokenTree struct tokentree
#define Node struct node
#define NodeList Node *

TokenTree{};

Node
{
	char *message;
	int op;
	Token *token;
	int count;
	Node **children;
};

void free_node(Node *node, int recursively);

Node *nnode(char *message, int op, Token *t, int children);

Node *token_node();

Node *sequence(Node *s1, Node *s2);

Node *unary_op(char *message, int op, Node *v1);

Node *binary_op(char *message, int op, Node *v1, Node *v2);

void print_node(Node *node, int indents);

#endif
