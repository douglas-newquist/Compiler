%{
	#include "main.h"
	#include "token.h"
	#include "rules.h"
	#include "tree.h"

	#define EMPTY_TREE tree("<EMPTY>", R_EMPTY, NULL, 0)

	extern int yylex();
%}

%union{
	struct tree* tree;
}

// Operators
%token <tree> INCREMENT DECREMENT AND OR

// Comparators
%token <tree> EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL

// Types
%token <tree> BOOLEAN CHAR DOUBLE INT

// Reserved words
%token <tree> BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF INSTANCEOF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE

// Literals
%token <tree> LITERAL_BOOL LITERAL_CHAR LITERAL_DOUBLE LITERAL_INT LITERAL_STRING LITERAL_NULL

// Generic token
%token <tree> ID

// Character tokens
%token <tree> '-'
%token <tree> '!'
%token <tree> '.'
%token <tree> '*'
%token <tree> '/'
%token <tree> '%'
%token <tree> '+'
%token <tree> '<'
%token <tree> '='
%token <tree> '>'

// Non-terminal tokens
%type <tree> FixedType
%type <tree> Literal
%type <tree> RelationOp
%type <tree> Visability

%type <tree> AnyType
%type <tree> Args
%type <tree> ArgDef
%type <tree> ArgDefs
%type <tree> Block
%type <tree> Break
%type <tree> Case
%type <tree> Cases
%type <tree> Class
%type <tree> ClassBody
%type <tree> ClassBodyDecl
%type <tree> ClassBodyDecls
%type <tree> Exp
%type <tree> Exp01
%type <tree> Exp03
%type <tree> Exp04
%type <tree> Exp08
%type <tree> Exp09
%type <tree> Exp11
%type <tree> Exp12
%type <tree> Exp13
%type <tree> Exp14
%type <tree> Exp15
%type <tree> ExpStatement
%type <tree> FieldAccess
%type <tree> For
%type <tree> ForCondition
%type <tree> ForIncrement
%type <tree> ForInit
%type <tree> IfStmt
%type <tree> IfThen
%type <tree> IfThenChain
%type <tree> IfThenChainElse
%type <tree> IfThenElse
%type <tree> Instantiate
%type <tree> Method
%type <tree> MethodCall
%type <tree> Name
%type <tree> Primary
%type <tree> Program
%type <tree> QualifiedName
%type <tree> Return
%type <tree> SingleType
%type <tree> Statement
%type <tree> Statements
%type <tree> Switch
%type <tree> SwitchBlock
%type <tree> Type
%type <tree> Value
%type <tree> VarDecl
%type <tree> VarDecls
%type <tree> VarDefs
%type <tree> While
%type <tree> ZeroStatments

%%

Program: Class { program=$$; }

Name	: ID
		| QualifiedName;

QualifiedName: Name '.' ID { $$=tree("Name", R_ACCESS1, $2, 2, $1, $3); };

AnyType	: VOID
		| Type;

Type: SingleType
	| Type '[' ']' { $$=tree("Array", R_ARRAY1, NULL, 1, $1); };

SingleType	: FixedType
			| Name;

FixedType: INT | DOUBLE | BOOLEAN | CHAR;

Literal	: LITERAL_INT
		| LITERAL_BOOL
		| LITERAL_STRING
		| LITERAL_CHAR
		| LITERAL_DOUBLE
		| LITERAL_NULL;

FieldAccess: Primary '.' ID { $$=tree("Access", R_ACCESS2, $2, 2, $1, $2); };

Visability: PUBLIC;

ArgDefs	: ArgDef
		| ArgDef ',' ArgDefs 	{ $$=group($1, $3); }
		| 						{ $$=EMPTY_TREE; };

ArgDef: Type ID { $$=tree("Define", R_DEFINE1, $2, 1, $1); };

Args: Args ',' Exp { $$=group($1, $3); };
	| Exp
	| { $$=EMPTY_TREE; }

// public class name { ... }
Class: Visability CLASS ID ClassBody { $$=tree("Class", R_CLASS1, $3, 1, $4); };

ClassBody	: '{' ClassBodyDecls '}'{ $$=$2; }
			| '{' '}' 				{ $$=EMPTY_TREE; };

ClassBodyDecls	: ClassBodyDecl
				| ClassBodyDecl ClassBodyDecls { $$=group($1, $2); };

ClassBodyDecl: VarDefs ';' | Method;

VarDefs: Type VarDecls { $$=tree("Variable", R_DEFINE2, NULL, 2, $1, $2); };

VarDecls	: VarDecls ',' VarDecl { $$=tree("Names", R_DEFINE3, NULL, 2, $1, $3); }
			| VarDecl;

VarDecl	: ID
		| ID '=' Exp	{ $$=tree("Define", R_DEFINE4, $1, 1, $3); };

// public static type name(args) { ... }
Method: Visability STATIC AnyType ID '(' ArgDefs ')' Block
		{ $$=tree("Method", R_METHOD1, $4, 4, $1, $3, $6, $8); };


Block: '{' ZeroStatments '}' { $$=$2; }

// Zero or more statments
ZeroStatments : Statements | { $$=EMPTY_TREE; }

Statements	: Statement
			| Statement Statements { $$=group($1, $2); };

Statement	: ';' { $$=EMPTY_TREE; }
			| Block
			| IfStmt
			| Switch
			| While
			| For
			| Return ';'
			| Break ';'
			| CONTINUE ';'
			| ExpStatement ';';

ExpStatement: MethodCall
			| VarDefs
			| Exp

MethodCall: Name '(' Args ')' { $$=tree("Call", R_CALL, NULL, 2, $1, $3); };

Switch: SWITCH '(' Exp ')' SwitchBlock { $$=tree("Switch", R_SWITCH, $1, 2, $3, $5); }

SwitchBlock	: '{' '}' 				{ $$=EMPTY_TREE; }
			| '{' Cases '}' 	{ $$=$2; }

Cases	: Case Cases { $$=group($1, $2); }
		| Case

Case	: CASE Literal ':' Statements { $$=tree("Case", R_CASE, $1, 2, $2, $4); }
		| DEFAULT ':' Statements { $$=tree("Default", R_DEFAULT_CASE, $1, 1, $3); }

While: WHILE '(' Exp ')' Block { $$=tree("While", R_WHILE, $1, 2, $3, $5); }

For: FOR '(' ForInit ';' ForCondition ';' ForIncrement ')' Block
	{ $$=tree("For", R_FOR, $1, 4, $3, $5, $7, $9); }

ForInit: VarDefs| { $$=EMPTY_TREE; }

ForCondition: Exp | { $$=EMPTY_TREE; }

ForIncrement:{ $$=EMPTY_TREE; }

IfStmt: IfThen | IfThenElse | IfThenChain | IfThenChainElse;

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfThenChainElse	: IfThenChain ELSE Block
				{ $$=tree("If+ Else", R_IF4, $2, 2, $1, $3); };

// if (condition) { ... } else if (condition) { ... } ...
IfThenChain	: IfThen ELSE IfThen 		{ $$=tree("If+", R_IF3, $2, 2, $1, $3); }
			| IfThenChain ELSE IfThen	{ $$=tree("If+", R_IF3, $2, 2, $1, $3); };

// if (condition) { ... } else { ... }
IfThenElse: IfThen ELSE Block { $$=tree("If Else", R_IF2, $2, 2, $1, $3); };

// if (condition) { ... }
IfThen: IF '(' Exp ')' Block { $$=tree("If", R_IF1, $1, 2, $3, $5); };

Break	: BREAK		{ $$=tree("Break", R_BREAK1, $1, 0); }
		| BREAK ID	{ $$=tree("Break", R_BREAK2, $1, 1, $2); }

Return	: RETURN Exp 	{ $$=tree("Return", R_RETURN2, $1, 1, $2); }
		| RETURN 		{ $$=tree("Return", R_RETURN1, $1, 0); };

Instantiate	: NEW Type '[' Exp ']' { $$=tree("New", R_ARRAY2, $1, 2, $2, $4); }
			| NEW Type '(' Args ')' { $$=tree("New", R_NEW1, $1, 2, $2, $4); };

Primary	: Literal
		| MethodCall
		| FieldAccess
		| '(' Exp ')'	{ $$=$2; };

Value	: Primary
		| Name;

Exp: Exp01;

Exp15	: Name INCREMENT { $$=tree("++", R_STEP, $2, 1, $1); }
		| Name DECREMENT { $$=tree("--", R_STEP, $2, 1, $1); }
		| Value;

Exp14	: '-' Exp14 { $$=tree("Negate", '-', $1, 1, $2); }
		| '!' Exp14 { $$=tree("Not", '!', $1, 1, $2); }
		| Exp15;

Exp13	: Instantiate
		| '(' Type ')' Name { $$=tree("Cast", R_CAST, NULL, 2, $2, $4); }
		| Exp14;

Exp12	: Exp12 '*' Exp14 { $$=tree("Mult", '*', $2, 2, $1, $3); }
		| Exp12 '/' Exp14 { $$=tree("Div", '/', $2, 2, $1, $3); }
		| Exp12 '%' Exp14 { $$=tree("Mod", '%', $2, 2, $1, $3); }
		| Exp13;

Exp11	: Exp11 '+' Exp12 { $$=tree("Add", '+', $2, 2, $1, $3); }
		| Exp11 '-' Exp12 { $$=tree("Sub", '-', $2, 2, $1, $3); }
		| Exp12;

Exp09	: Exp09 RelationOp Exp11
		{ $$=tree("Compare", $2->token->category, $2, 2, $1, $3); }
		| Name INSTANCEOF Type { $$=tree("Is", R_INSTANCE, $2, 2, $1, $3); }
		| Exp11;

Exp08	: Exp08 EQUALS Exp09  	{ $$=tree("Equal", R_EQUALS, $2, 2, $1, $3); }
		| Exp08 NOT_EQUAL Exp09 { $$=tree("Not Equal", R_NOT_EQUAL, $2, 2, $1, $3); }
		| Exp09;

Exp04	: Exp04 AND Exp08 { $$=tree("And", R_AND, $2, 2, $1, $3); };
		| Exp08;

Exp03	: Exp03 OR Exp04 { $$=tree("Or", R_AND, $2, 2, $1, $3); }
		| Exp04;

Exp01	: Exp03;

RelationOp: '<' | '>' | LESS_EQUAL | GREATER_EQUAL;
