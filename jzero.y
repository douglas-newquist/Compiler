%{
	#include "main.h"
	#include "token.h"
	#include "tree.h"
%}

%union{
	struct node *node;
	struct token *token;
	int integer;
}

// Operators
%token <integer> INCREMENT DECREMENT AND NOT OR
// Comparators
%token <integer> EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL
// Types
%token <integer> BOOLEAN CHAR DOUBLE INT
// Reserved words
%token <integer> BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF INSTANCEOF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE
// Literals
%token <integer> LITERAL_BOOL LITERAL_CHAR LITERAL_DOUBLE LITERAL_INT LITERAL_STRING LITERAL_NULL
%token <integer> ID

%type <node> Literal
%type <node> VarDef
%type <node> AnyType Type ListType SimpleType
%type <node> Name QualifiedName
%type <node> Exp Negated
%type <node> Value Create Program

%start Program
%%
Program	: Create { $$=$1; }
		| Create Program { program=sequence($1, $2); $$=program; };

Create: VarDef '=' Exp ';' { $$=binary_op("Assign", '=', $1, $3); };

VarDef: Type Name {  $$=binary_op("Define", DECLARE, $1, $2); };

Value: Literal	{ $$=token_node(); };

Literal	: LITERAL_BOOL		{ $$=token_node(); }
		| LITERAL_CHAR		{ $$=token_node(); }
		| LITERAL_STRING	{ $$=token_node(); }
		| LITERAL_INT		{ $$=token_node(); }
		| LITERAL_DOUBLE	{ $$=token_node(); }
		| LITERAL_NULL		{ $$=token_node(); };

// Non-void or void type
AnyType	: Type { $$=$1; }
		| VOID { $$=token_node(); };

// Non-void type
Type: SimpleType
	| ListType;

ListType: SimpleType '[' ']'
		| ListType '[' ']';

SimpleType	: BOOLEAN	{ $$=token_node(); }
			| CHAR		{ $$=token_node(); }
			| INT 		{ $$=token_node(); }
			| DOUBLE	{ $$=token_node(); };

Name: ID				{ $$=token_node(); }
	| QualifiedName;

QualifiedName: Name '.' ID;

Exp: Value | Negated | '(' Exp ')';
Negated: '-' Exp { $$=unary_op("Negate", '-', $2); };
