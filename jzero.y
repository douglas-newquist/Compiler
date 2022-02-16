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
%type <node> NamesList
%type <node> VarDef VarsDef
%type <node> AnyType Type ArrayType SingleType SimpleType
%type <node> Name QualifiedName
%type <node> Exp Negated
%type <node> Value Create Creates Class ClassBody ClassBodyDef Program
%type <node> Id
%type <node> ParamsDef
%type <node> Method

%start Program
%%
Program: Class { program=$$; };

Class: PUBLIC CLASS Id '{' ClassBody '}' { $$=binary_op("Class", DECLARE, $3, $5); };
ClassBody	: { $$=NULL; }
			| ClassBodyDef ClassBody { $$=sequence($1, $2); };
ClassBodyDef
			: Method
			| VarDef ';' { $$=$1; }
			| VarsDef ';' { $$=$1; }
			| Create;

Creates: Create | Create Creates { $$=sequence($1, $2); };
Create: VarDef '=' Exp ';' { $$=binary_op("Assign", '=', $1, $3); };

Method: PUBLIC STATIC AnyType Id '(' ParamsDef ')' '{' '}' { $$=trinary_op("Method", DECLARE, $3, $4, $6); }
ParamsDef:{$$=NULL;}	| VarDef
			| VarDef ',' ParamsDef { $$=binary_op("Params", DECLARE, $1, $3); };

NamesList	: Name
			| Name ',' NamesList { $$=binary_op("Names", DECLARE, $1, $3); }

VarsDef: Type NamesList { $$=binary_op("Define Vars", DECLARE, $1, $2); }
VarDef: Type Name {  $$=binary_op("Define", DECLARE, $1, $2); };

Value: Literal;

Literal	: LITERAL_BOOL		{ $$=token_node(); }
		| LITERAL_CHAR		{ $$=token_node(); }
		| LITERAL_STRING	{ $$=token_node(); }
		| LITERAL_INT		{ $$=token_node(); }
		| LITERAL_DOUBLE	{ $$=token_node(); }
		| LITERAL_NULL		{ $$=token_node(); };

// Non-void or void type
AnyType	: Type
		| VOID { $$=token_node(); };

// Non-void type
Type: SingleType
	| ArrayType;

ArrayType: SingleType '[' ']'	{ $$=unary_op("Array", '[', $1); }
		| ArrayType '[' ']'		{ $$=unary_op("Array", '[', $1); };

SingleType	: SimpleType
			| Name

SimpleType	: BOOLEAN	{ $$=token_node(); }
			| CHAR		{ $$=token_node(); }
			| INT 		{ $$=token_node(); }
			| DOUBLE	{ $$=token_node(); };

Id: ID { $$=token_node(); }
Name: Id | QualifiedName;

QualifiedName: Name '.' Id { $$=binary_op("Access", '.', $1, $3); };

Exp: Value | Negated | '(' Exp ')';
Negated: '-' Exp { $$=unary_op("Negate", '-', $2); };
