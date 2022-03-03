%{
	#include "main.h"
	#include "token.h"
	#include "rules.h"
	#include "tree.h"

#if DEBUG
#define YYDEBUG 1
#endif

	#define EMPTY_TREE tree("<EMPTY>", R_EMPTY, NULL, 0)

	extern int yylex();
%}

%union{
	struct token* token;
	struct tree* tree;
}

// Operators
%token <tree> INCREMENT DECREMENT AND OR

// Comparators
%token <tree> EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL

// Types
%token <tree> BOOLEAN CHAR DOUBLE INT

// Reserved words
%token <tree> BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE

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
%type <tree> Owner

%type <tree> AnyType
%type <tree> ArgDef
%type <tree> ArgDefs
%type <tree> Args
%type <tree> ArrayAccess
%type <tree> Assignment
%type <tree> Block
%type <tree> BreakExp
%type <tree> Case
%type <tree> Cases
%type <tree> Class
%type <tree> ClassBody
%type <tree> ClassBodyDecl
%type <tree> ClassBodyDecls
%type <tree> Constructor
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
%type <tree> ExpList
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
%type <tree> MethodHead
%type <tree> MethodCall
%type <tree> Name
%type <tree> Primary
%type <tree> Program
%type <tree> QualifiedName
%type <tree> ReturnExp
%type <tree> SingleType
%type <tree> Statement
%type <tree> Statements
%type <tree> Step
%type <tree> Switch
%type <tree> SwitchBlock
%type <tree> Type
%type <tree> Value
%type <tree> VarDecl
%type <tree> VarDecls
%type <tree> VarDefs
%type <tree> While
%type <tree> ZeroArgDefs
%type <tree> ZeroArgs
%type <tree> ZeroStatments

%start Program

%%

Program: Class { program=$1; }

Name: ID | QualifiedName;

QualifiedName: Name '.' ID { $$=tree("Name", R_ACCESS1, $3, 1, $1); };

AnyType: VOID | Type;

Type: SingleType
	| Type '[' ']' { $$=tree("Array", R_ARRAY1, NULL, 1, $1); };

SingleType: FixedType | Name;

FixedType: INT | DOUBLE | BOOLEAN | CHAR;

Literal	: LITERAL_INT
		| LITERAL_BOOL
		| LITERAL_STRING
		| LITERAL_CHAR
		| LITERAL_DOUBLE
		| LITERAL_NULL;

FieldAccess: Primary '.' ID { $$=tree("Field", R_ACCESS2, $3, 1, $1); };

Visability: PUBLIC;

Owner: STATIC;

// Zero or more arg defs
ZeroArgDefs: ArgDefs | { $$=EMPTY_TREE; }

ArgDefs	: ArgDef ',' ArgDefs { $$=group("Params", R_ARG_DEF_GROUP, $1, $3); }
		| ArgDef;

ArgDef: Type ID { $$=tree("Define", R_DEFINE1, $2, 1, $1); };

// Zero or more args
ZeroArgs: Args | { $$=EMPTY_TREE; };

Args: Args ',' Exp { $$=group("Args", R_ARG_GROUP, $1, $3); };
	| Exp;

// public class name { ... }
Class: Visability CLASS ID ClassBody { $$=tree("Class", R_CLASS1, $3, 1, $4); };

ClassBody	: '{' ClassBodyDecls '}'{ $$=$2; }
			| '{' '}' 				{ $$=EMPTY_TREE; };

ClassBodyDecls	: ClassBodyDecl
				| ClassBodyDecl ClassBodyDecls
				{ $$=group("Group", R_CLASS_GROUP, $1, $2); };

ClassBodyDecl: VarDefs ';' | Method | Constructor;

VarDefs: Type VarDecls { $$=tree("Variable", R_DEFINE2, NULL, 2, $1, $2); };

VarDecls	: VarDecl ',' VarDecls
			{ $$=group("Vars", R_VAR_GROUP, $1, $3); }
			| VarDecl;

VarDecl	: ID
		| ID '=' Exp	{ $$=tree("Let", R_DEFINE3, $1, 1, $3); };

// public static type name(args) { ... }
Method	: Visability Owner MethodHead
		{ $$=tree("Method", R_METHOD2, NULL, 3, $1, $2, $3); }
		| Visability MethodHead
		{ $$=tree("Method", R_METHOD2, NULL, 3, $1, EMPTY_TREE, $2); }
		| Owner MethodHead
		{ $$=tree("Method", R_METHOD2, NULL, 3, EMPTY_TREE, $1, $2); }
		| MethodHead
		{ $$=tree("Method", R_METHOD2, NULL, 3, EMPTY_TREE, EMPTY_TREE, $1); }

MethodHead	: AnyType ID '(' ZeroArgDefs ')' Block
			{ $$=tree("Head", R_METHOD1, $2, 3, $1, $4, $6); }

Constructor	: Visability ID '(' ZeroArgDefs ')' Block
			{ $$=tree("Constructor", R_METHOD3, $2, 3, $1, $4, $6); }

Block: '{' ZeroStatments '}' { $$=$2; }

// Zero or more statments
ZeroStatments : Statements | { $$=EMPTY_TREE; }

Statements	: Statement Statements
			{ $$=group("Statements", R_STATEMENT_GROUP, $1, $2); }
			| Statement;

Statement	: ';' { $$=EMPTY_TREE; }
			| Block
			| IfStmt
			| Switch
			| While
			| For
			| ReturnExp ';'
			| BreakExp ';'
			| CONTINUE ';'
			| ExpStatement ';';

Assignment: Name '=' Exp { $$=tree("Assign", R_ASSIGN, $2, 2, $1, $3); }

ExpStatement: MethodCall
			| VarDefs
			| Step
			| Assignment
			| Instantiate

ExpList	: ExpList ',' ExpStatement { $$=group("Exps", R_EXP_GROUP, $1, $3); }
		| ExpStatement

MethodCall	: Name '(' ZeroArgs ')' 		{ $$=tree("Call", R_CALL1, NULL, 2, $1, $3); }
			| FieldAccess '(' ZeroArgs ')' 	{ $$=tree("Call", R_CALL2, NULL, 2, $1, $3); }

Switch: SWITCH '(' Exp ')' SwitchBlock { $$=tree("Switch", R_SWITCH, $1, 2, $3, $5); }

SwitchBlock	: '{' '}' 			{ $$=EMPTY_TREE; }
			| '{' Cases '}' 	{ $$=$2; }

Cases	: Cases Case { $$=group("Cases", R_CASE_GROUP, $1, $2); }
		| Case

Case	: CASE Literal ':' Statements { $$=tree("Case", R_CASE, $2, 1, $4); }
		| DEFAULT ':' Statements { $$=tree("Default", R_DEFAULT_CASE, $1, 1, $3); }

While: WHILE '(' Exp ')' Block { $$=tree("While", R_WHILE, $1, 2, $3, $5); }

For: FOR '(' ForInit ';' ForCondition ';' ForIncrement ')' Block
	{ $$=tree("For", R_FOR, $1, 4, $3, $5, $7, $9); }

ForInit: ExpList | { $$=EMPTY_TREE; }

ForCondition: Exp | { $$=EMPTY_TREE; }

ForIncrement: ExpList | { $$=EMPTY_TREE; }

IfStmt: IfThen | IfThenElse | IfThenChain | IfThenChainElse;

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfThenChainElse	: IfThenChain ELSE Block
				{ $$=tree("If+ Else", R_IF2, $2, 2, $1, $3); };

// if (condition) { ... } else if (condition) { ... } ...
IfThenChain	: IfThen ELSE IfThen 		{ $$=tree("If+", R_IF3, $2, 2, $1, $3); }
			| IfThenChain ELSE IfThen	{ $$=tree("If+", R_IF3, $2, 2, $1, $3); };

// if (condition) { ... } else { ... }
IfThenElse: IfThen ELSE Block { $$=tree("If Else", R_IF2, $2, 2, $1, $3); };

// if (condition) { ... }
IfThen: IF '(' Exp ')' Block { $$=tree("If", R_IF1, $1, 2, $3, $5); };

BreakExp: BREAK		{ $$=tree("Break", R_BREAK1, $1, 0); }
		| BREAK ID	{ $$=tree("Break", R_BREAK2, $1, 1, $2); }

ReturnExp	: RETURN 		{ $$=tree("Return", R_RETURN1, $1, 0); }
			| RETURN Exp 	{ $$=tree("Return", R_RETURN2, $1, 1, $2); };

Instantiate	: NEW Type '[' Exp ']' 		{ $$=tree("New Array", R_ARRAY2, $1, 2, $2, $4); }
			| NEW Type '(' ZeroArgs ')' { $$=tree("New", R_NEW1, $1, 2, $2, $4); };

ArrayAccess	: Name '[' Exp ']'		{ $$=tree("Index", R_ACCESS3, NULL, 2, $1, $3); }
			| Primary '[' Exp ']'	{ $$=tree("Index", R_ACCESS3, NULL, 2, $1, $3); }

Primary	: Literal
		| MethodCall
		| FieldAccess
		| '(' Exp ')'	{ $$=$2; };

Value	: Primary
		| ArrayAccess
		| Name;

Step: Name INCREMENT { $$=tree("++", R_UNARY_OP2, $2, 1, $1); }
	| Name DECREMENT { $$=tree("--", R_UNARY_OP2, $2, 1, $1); }

Exp: Exp01;

Exp15	: Step
		| Value;

Exp14	: '-' Exp14 { $$=tree("Negate", R_UNARY_OP1, $1, 1, $2); }
		| '!' Exp14 { $$=tree("Not", R_UNARY_OP1, $1, 1, $2); }
		| Exp15;

Exp13	: Instantiate
		| Exp14;

Exp12	: Exp12 '*' Exp14 { $$=tree("Mult", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp12 '/' Exp14 { $$=tree("Div", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp12 '%' Exp14 { $$=tree("Mod", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp13;

Exp11	: Exp11 '+' Exp12 { $$=tree("Add", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp11 '-' Exp12 { $$=tree("Sub", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp12;

Exp09	: Exp09 RelationOp Exp11
		{ $$=tree("Compare", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp11;

Exp08	: Exp08 EQUALS Exp09  	{ $$=tree("Equal", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp08 NOT_EQUAL Exp09 { $$=tree("Not Equal", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp09;

Exp04	: Exp04 AND Exp08 { $$=tree("And", R_BINARY_OP, $2, 2, $1, $3); };
		| Exp08;

Exp03	: Exp03 OR Exp04 { $$=tree("Or", R_BINARY_OP, $2, 2, $1, $3); }
		| Exp04;

Exp01	: Exp03;

RelationOp: '<' | '>' | LESS_EQUAL | GREATER_EQUAL;
