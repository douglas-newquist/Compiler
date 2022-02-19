%{
	#include "main.h"
	#include "token.h"
	#include "rules.h"
	#include "tree.h"

	#define EMPTY_TREE tree("<EMPTY>", R_EMPTY, NULL, 0)
	#define group(a, b) tree("Group", R_GROUP, NULL, 2, a, b);

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

// Generic token
%token <token> ID

// Character tokens
%token <token> '-'
%token <token> '!'
%token <token> '.'
%token <token> '*'
%token <token> '/'
%token <token> '%'
%token <token> '+'
%token <token> '<'
%token <token> '='
%token <token> '>'

// Non-terminal tokens
%type <token> FixedType
%type <token> Literal
%type <token> RelationOp
%type <token> Visability

%type <tree> AddSub
%type <tree> AndExp
%type <tree> AnyType
%type <tree> Arg Args
%type <tree> ArgDef
%type <tree> ArgDefs
%type <tree> Block
%type <tree> Class
%type <tree> ClassBody
%type <tree> ClassBodyDecl
%type <tree> ClassBodyDecls
%type <tree> EqualExp
%type <tree> Exp
%type <tree> Field
%type <tree> FieldAccess
%type <tree> FieldDecl
%type <tree> FieldDecls
%type <tree> IfStmt
%type <tree> IfThen
%type <tree> IfThenChain
%type <tree> IfThenChainElse
%type <tree> IfThenElse
%type <tree> Instantiate
%type <tree> Method
%type <tree> MethodCall
%type <tree> MultDiv
%type <tree> Name
%type <tree> OrExp
%type <tree> Program
%type <tree> QualifiedName
%type <tree> Relation
%type <tree> Return
%type <tree> SingleType
%type <tree> Statement
%type <tree> Statements
%type <tree> Step
%type <tree> Switch
%type <tree> SwitchBlock
%type <tree> SwitchCase
%type <tree> SwitchCaseBlock
%type <tree> SwitchCases
%type <tree> Type
%type <tree> Unary
%type <tree> Value

%%

Program: Class { program=$$; }

Name	: ID { $$=tree_token($1); }
		| QualifiedName;

QualifiedName: Name '.' ID { $$=tree("Name", 1000, $2, 2, $1, $3); };

AnyType	: VOID { $$=tree_token($1); }
		| Type;

Type: SingleType
	| Type '[' ']' { $$=tree("Array", R_ARRAY1, NULL, 1, $1); };

SingleType	: FixedType { $$=tree_token($1); }
			| Name;

FixedType: INT | DOUBLE | BOOLEAN | CHAR;

Literal	: LITERAL_INT
		| LITERAL_BOOL
		| LITERAL_STRING
		| LITERAL_CHAR
		| LITERAL_DOUBLE
		| LITERAL_NULL;

FieldAccess: Value '.' ID { $$=tree("Access", 1000, $2, 2, $1, $2); };

Visability: PUBLIC;

ArgDefs	: ArgDef
		| ArgDef ',' ArgDefs 	{ $$=group($1, $3); }
		| 						{ $$=EMPTY_TREE; };

ArgDef: Type ID { $$=tree("Define", 1000, $2, 1, $1); };

Args: { $$=EMPTY_TREE; }
	| Arg
	| Args ',' Arg { $$=group($1, $3); };

Arg: Exp;

// public class name { ... }
Class: Visability CLASS ID ClassBody { $$=tree("Class", R_CLASS1, $3, 1, $4); };

ClassBody	: '{' ClassBodyDecls '}'{ $$=$2; }
			| '{' '}' 				{ $$=EMPTY_TREE; };

ClassBodyDecls	: ClassBodyDecl
				| ClassBodyDecl ClassBodyDecls { $$=group($1, $2); };

ClassBodyDecl: Field | Method;

// type something = value;
Field: Type FieldDecls ';' { $$=tree("Field", R_FIELD1, NULL, 2, $1, $2); };

FieldDecls	: FieldDecl ',' FieldDecls { $$=tree("Names", R_FIELD3, NULL, 2, $1, $3); }
			| FieldDecl;

FieldDecl	: ID			{ $$=tree_token($1); }
			| ID '=' Exp	{ $$=tree("Let", R_FIELD2, $1, 1, $3); };

// public static type name(args) { ... }
Method: Visability STATIC AnyType ID '(' ArgDefs ')' Block
		{ $$=tree("Method", R_METHOD1, $4, 4, $1, $3, $6, $8); };


Block	: '{' Statements '}'{ $$=tree("Block", 1000, NULL, 1, $2); }
		| '{' '}' 			{ $$=EMPTY_TREE; };


Statements	: Statement
			| Statement Statements { $$=group($1, $2); };

Statement	: ';' { $$=EMPTY_TREE; }
			| Block
			| IfStmt
			| Switch
			| Return
			| MethodCall ';'
			| Field
			| Step ';';

MethodCall: Name '(' Args ')' { $$=tree("Call", R_CALL1, NULL, 2, $1, $3); };

Switch: SWITCH '(' Exp ')' SwitchBlock { $$=tree("Switch", 1000, $1, 2, $3, $5); }

SwitchBlock	: '{' '}' 				{ $$=EMPTY_TREE; }
			| '{' SwitchCases '}' 	{ $$=$2; }

SwitchCases	: SwitchCase SwitchCases { $$=tree("Cases", 1000, NULL, 2, $1, $2); }
			| SwitchCase

SwitchCase	: CASE Literal ':' SwitchCaseBlock { $$=tree("Case", 1000, $1, 2, $2, $4); }
			| DEFAULT ':' SwitchCaseBlock { $$=tree("Case", 1000, $1, 1, $3); }

SwitchCaseBlock	: Statements BREAK ';' 	{ $$=$1; }
				| Statements Return		{ $$=tree("Return Case", 1000, NULL, 2, $1, $2); }

IfStmt: IfThen | IfThenElse ;

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfThenChainElse: IfThenChain ELSE IfThenElse
					{ $$=tree("If+ Else", R_IF4, $2, 2, $1, $3); };

// FIXME If chains not working
// if (condition) { ... } else if (condition) { ... } ...
IfThenChain	: IfThen ELSE IfThenChain { $$=tree("If+", R_IF3, $2, 2, $1, $3); }
			| IfThen;

// if (condition) { ... } else { ... }
IfThenElse: IF '(' Exp ')' Block ELSE Block { $$=tree("If Else", R_IF2, $1, 3, $3, $5, $7); };

// if (condition) { ... }
IfThen: IF '(' Exp ')' Block { $$=tree("If", R_IF1, $1, 2, $3, $5); };

Return	: RETURN Exp ';' 	{ $$=tree("Return", R_RETURN2, $1, 1, $2); }
		| RETURN ';'		{ $$=tree("Return", R_RETURN1, $1, 0); };

Instantiate	: NEW Type '[' Exp ']' { $$=tree("New", R_ARRAY2, $1, 2, $2, $4); }
			| NEW Type '(' Args ')' { $$=tree("New", R_NEW1, $1, 2, $2, $4); };

Value	: Literal 		{ $$=tree_token($1); }
		| Instantiate
		| MethodCall
		| Name
		| FieldAccess
		| '(' Exp ')'	{ $$=$2; };

Exp: Value | OrExp;

Step: Name INCREMENT { $$=tree("++", 1000, $2, 1, $1); }
	| Name DECREMENT { $$=tree("--", 1000, $2, 1, $1); };

OrExp	: OrExp OR AndExp { $$=tree("Or", R_AND, $2, 2, $1, $3); };
		| AndExp

AndExp	: AndExp AND EqualExp { $$=tree("And", R_AND, $2, 2, $1, $3); };
		| EqualExp

EqualExp: EqualExp EQUALS Relation  	{ $$=tree("Equal", R_EQUALS, $2, 2, $1, $3); }
		| EqualExp NOT_EQUAL Relation 	{ $$=tree("Not Equal", R_NOT_EQUAL, $2, 2, $1, $3); }
		| Relation;

RelationOp: '<' | '>' | LESS_EQUAL | GREATER_EQUAL;

Relation: Relation RelationOp AddSub
		{ $$=tree("Compare", $2->category, $2, 2, $1, $3); }
		| AddSub;

AddSub	: AddSub '+' MultDiv { $$=tree("Add", '+', $2, 2, $1, $3); }
		| AddSub '-' MultDiv { $$=tree("Sub", '-', $2, 2, $1, $3); }
		| MultDiv;

MultDiv	: MultDiv '*' Unary { $$=tree("Mult", '*', $2, 2, $1, $3); }
		| MultDiv '/' Unary { $$=tree("Div", '/', $2, 2, $1, $3); }
		| MultDiv '%' Unary { $$=tree("Mod", '%', $2, 2, $1, $3); }
		| Unary;

Unary	: '-' Unary { $$=tree("Negate", '-', $1, 1, $2); }
		| '!' Unary { $$=tree("Not", '!', $1, 1, $2); }
		| Value;
