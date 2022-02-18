%{
	#include "main.h"
	#include "tree.h"

	extern int yylex();
%}

%union{
	struct tree *tree;
	struct token *token;
}

// Operators
%token <token> INCREMENT DECREMENT AND NOT OR
// Comparators
%token <token> EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL
// Types
%token <token> BOOLEAN CHAR DOUBLE INT
// Reserved words
%token <token> BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF INSTANCEOF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE
// Literals
%token <token> LITERAL_BOOL LITERAL_CHAR LITERAL_DOUBLE LITERAL_INT LITERAL_STRING LITERAL_NULL
%token <token> ID

%type <token> Literal
%type <tree> NamesList
%type <tree> VarDef VarsDef
%type <tree> AnyType Type ArrayType SingleType SimpleType
%type <tree> Name QualifiedName
%type <tree> Exp Negated
%type <tree> Value Create Creates Class ClassBody ClassBodyDef Program
%type <tree> ParamsDef
%type <tree> Method

%type <token> '-'

%start Program
%%
Program: Class { yylval.tree=$$; };

Class: PUBLIC CLASS ID '{' ClassBody '}' { $$=tree("Class", 1000, NULL, 3, $1, $3, $5); };
ClassBody	: { $$=tree("Block", 1000, NULL, 0); }
			| ClassBodyDef ClassBody { $$=tree("Block", 1000, NULL, 2, $1, $2); };
ClassBodyDef
			: Method
			| VarDef ';' { $$=$1; }
			| VarsDef ';' { $$=$1; }
			| Create;

Creates: Create | Create Creates { $$=tree("Chain", 1000, NULL, 2, $1, $2); };
Create: VarDef '=' Exp ';' { $$=tree("Assign", 1000, NULL, 2, $1, $3); };

Method: PUBLIC STATIC AnyType ID '(' ParamsDef ')' '{' '}' { $$=tree("Method", 1000, NULL, 4, $1, $3, $4, $6); }
ParamsDef:{$$=NULL;}	| VarDef
			| VarDef ',' ParamsDef { $$=tree("Params", 1000, NULL, 2, $1, $3); };

NamesList	: Name
			| Name ',' NamesList { $$=tree("Names", 1000, NULL, 2, $1, $3); }

VarsDef: Type NamesList { $$=tree("Define Vars", 1000, NULL, 2, $1, $2); }
VarDef: Type Name {  $$=tree("Define", 1000, NULL, 2, $1, $2); };

Value: Literal;

Literal	: LITERAL_BOOL
		| LITERAL_CHAR
		| LITERAL_STRING
		| LITERAL_INT
		| LITERAL_DOUBLE
		| LITERAL_NULL;

// Non-void or void type
AnyType	: Type
		| VOID ;

// Non-void type
Type: SingleType
	| ArrayType;

ArrayType: SingleType '[' ']'	{ $$=tree("Array", 1000, NULL, 1, $1); }
		| ArrayType '[' ']'		{ $$=tree("Array", 1000, NULL, 1, $1); };

SingleType	: SimpleType
			| Name

SimpleType	: BOOLEAN
			| CHAR
			| INT
			| DOUBLE;

Name: ID | QualifiedName;

QualifiedName: Name '.' ID { $$=tree("Access", 1000, NULL, 2, $1, $3); };

Exp: Value | Negated | '(' Exp ')';
Negated: '-' Exp { $$=tree("Negate", 1000, $1, 1, $2); };
