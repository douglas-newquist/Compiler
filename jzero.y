%{
	#include "main.h"
	#include "token.h"
	#include "tree.h"

	extern int yylex();
%}

%union{
	struct token *token;
	struct tree*tree;
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

%token <token> '.' '='

%type <token> FixedType
%type <token> Literal
%type <token> Visability
%type <tree> Arg Args
%type <tree> ArgDef ArgsDef
%type <tree> Block
%type <tree> Class ClassBody ClassBodyDecl ClassBodyDecls
%type <tree> Exp
%type <tree> Field
%type <tree> FieldAccess
%type <tree> FieldDecl FieldDecls
%type <tree> IfThen IfThenElse IfThenElseIf IfElseIfElse
%type <tree> Instantiate
%type <tree> Method
%type <tree> MethodCall
%type <tree> Name
%type <tree> Program
%type <tree> QualifiedName
%type <tree> Return
%type <tree> SingleType Type AnyType
%type <tree> Statement Statements
%type <tree> Step
%type <tree> Value

%%

Program: Class { program=$$; }

Name	: ID { $$=tree_token($1); }
		| QualifiedName;

QualifiedName: Name '.' ID { $$=tree("Name", 1998, $2, 2, $1, $3); };

AnyType	: VOID { $$=tree_token($1); }
		| Type;

Type: SingleType
	| Type '[' ']' { $$=tree("Array", 1865, NULL, 1, $1); };

SingleType	: FixedType { $$=tree_token($1); }
			| Name;

FixedType: INT | DOUBLE | BOOLEAN | CHAR;

Value	: Literal		{ $$=tree_token($1); }
		| Instantiate;

Literal	: LITERAL_INT
		| LITERAL_BOOL
		| LITERAL_STRING
		| LITERAL_CHAR
		| LITERAL_DOUBLE
		| LITERAL_NULL;

FieldAccess: Value '.' ID;

Visability: PUBLIC;

ArgsDef	: ArgDef
		| ArgsDef ',' ArgDef 	{ $$=tree("Args", 1829, NULL, 2, $1, $3); }
		| 						{ $$=tree("Args", 1908, NULL, 0); };

ArgDef: Type ID { $$=tree("Define", 1365, $2, 1, $1); };

Args: { $$=EMPTY_TREE; } | Arg | Args ',' Arg;
Arg: Value;

// public class name { ... }
Class: Visability CLASS ID ClassBody { $$=tree("Class", 1196, $3, 1, $4); };

ClassBody	: '{' ClassBodyDecls '}'{ $$=$2; }
			| '{' '}' 				{ $$=tree("Block", 1990, NULL, 0); };

ClassBodyDecls	: ClassBodyDecl
				| ClassBodyDecls ClassBodyDecl { $$=tree("Chain", 1049, NULL, 2, $1, $2); };

ClassBodyDecl: Field | Method;

// type something = value;
Field: Type FieldDecls ';' { $$=tree("Field", 1803, NULL, 2, $1, $2); };

FieldDecls	: FieldDecl
			| FieldDecls ',' FieldDecl { $$=tree("Names", 1199, NULL, 2, $1, $3); }

FieldDecl	: ID			{ $$=tree_token($1); }
			| ID '=' Exp	{ $$=tree("Let", 1873, $1, 1, $3); }

// public static type name(args) { ... }
Method: Visability STATIC AnyType ID '(' ArgsDef ')' Block
		{ $$=tree("Method", 1825, NULL, 5, $1, $3, $4, $6, $8); };


Block	: '{' Statements '}'{ $$=tree("Block", 1711, NULL, 1, $2); }
		| '{' '}' 			{ $$=tree("Block", 1591, NULL, 0); };


Statements: Statement | Statements Statement { $$=tree("Chain", 1049, NULL, 2, $1, $2); };
Statement: ';' { $$=tree("DNO", 0, NULL, 0); } | Block | IfThen | IfThenElse | IfThenElseIf | IfElseIfElse | Return | MethodCall | Field;

MethodCall: Name '(' Args ')' ';' { $$=tree("Call", 1879, NULL, 2, $1, $3); }

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfElseIfElse: IfThenElseIf ELSE Block;
// if (condition) { ... } else if (condition) { ... } ...
IfThenElseIf: IfThen ELSE IfThen | IfThenElseIf ELSE IfThen;
// if (condition) { ... } else { ... }
IfThenElse: IfThen ELSE Block { $$=tree("IfE", 1587, $2, 2, $1, $3); };
// if (condition) { ... }
IfThen: IF '(' Exp ')' Block { $$=tree("If", 1111, $1, 2, $3, $5); }

Return	: RETURN Exp ';' 	{ $$=tree("Return", 1934, $1, 1, $2); }
		| RETURN ';'		{ $$=tree("Return", 1283, $1, 0); }

Instantiate: NEW Type '[' Exp ']' { $$=tree("New", 1361, $1, 2, $2, $4); }

Exp	:  Value
	| Name
	| '(' Exp ')' 	{ $$=$2; };

Step: Name INCREMENT { $$=tree("++", 1131, $2, 1, $1); }
	| Name DECREMENT { $$=tree("--", 1131, $2, 1, $1); };
