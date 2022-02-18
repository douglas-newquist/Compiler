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

%type <tree> Program
%type <tree> Class ClassBody ClassBodyDecl ClassBodyDecls
%type <token> Visability
%type <token> FixedType
%type <tree> SingleType ListType Type AnyType
%type <tree> Value
%type <tree> Exp
%type <tree> ArgDef ArgsDef
%type <tree> Arg Args
%type <tree> Field
%type <tree> VarDef VarsDef
%type <tree> Method MethodHead
%type <tree> Block
%type <tree> Statement Statements
%type <tree> IfThen IfThenElse IfThenElseIf IfElseIfElse
%type <tree> Name
%type <token> Literal
%type <tree> Return
%type <tree> Step
%type <tree> Instantiate

%%
Program: Class { program=$$; }

Name	: ID { $$=tree_token($1); }
		| QualifiedName;
QualifiedName: Name '.' ID;

AnyType	: VOID { $$=tree_token($1); }
		| Type;

Type: SingleType | ListType;

ListType: SingleType '[' ']' 	{ $$=tree("Array", 1865, NULL, 1, $1); }
		| ListType '[' ']'		{ $$=tree("Array", 1114, NULL, 1, $1); };

SingleType	: FixedType
			| Name;

FixedType: INT | DOUBLE | BOOLEAN | CHAR;

Value	: Literal		{ $$=tree_token($1); }
		| Exp
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
Args: Arg | Args ',' Arg;
Arg: Value;

// public class name { ... }
Class: Visability CLASS ID ClassBody { $$=tree("Class", 1196, $3, 1, $4); };

ClassBody	: '{' ClassBodyDecls '}'{ $$=$2; }
			| '{' '}' 				{ $$=tree("Block", 1990, NULL, 0); };

ClassBodyDecls	: ClassBodyDecl
				| ClassBodyDecls ClassBodyDecl { $$=tree("Chain", 1049, NULL, 2, $1, $2); };

ClassBodyDecl: Field | Method;

// type something = value;
Field: Type VarsDef ';';
VarsDef: VarDef | VarsDef ',' VarDef;
VarDef: ID | VarDef '[' ']';


// public static type name(args) { ... }
Method: Visability STATIC AnyType ID '(' ArgsDef ')' Block
		{ $$=tree("Method", 1825, NULL, 5, $1, $3, $4, $6, $8); };


Block	: '{' Statements '}'{ $$=tree("Block", 1711, NULL, 1, $2); }
		| '{' '}' 			{ $$=tree("Block", 1591, NULL, 0); };


Statements: Statement | Statements Statement;
Statement: ';' | Block | IfThen | IfThenElse | IfThenElseIf | IfElseIfElse | WhileStmt | For | BREAK ';' | Return | QualifiedName '(' Literal ')' ';';

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfElseIfElse: IfThenElseIf ELSE Block;
// if (condition) { ... } else if (condition) { ... } ...
IfThenElseIf: IfThen ELSE IfThen | IfThenElseIf ELSE IfThen;
// if (condition) { ... } else { ... }
IfThenElse: IfThen ELSE Block;
// if (condition) { ... }
IfThen: IF '(' Exp ')' Block

// while (conditon) { ... }
WhileStmt: WHILE '(' Exp ')' Block;

Return	: RETURN Exp ';' 	{ $$=tree("Return", 1009, $1, 1, $2); }
		| RETURN ';'		{ $$=tree("Return", 1100, $1, 0); };
SwitchCase: CASE Value ':' Statements BREAK ';';
Continue: CONTINUE ';';
Default: DEFAULT ':';

For: FOR '(' ForInit ';' ForCond ';' ForInc  ')' Block
ForInit: VarDef | ;
ForCond: Condition | ;
ForInc: Statement | ;

Instantiate: NEW Type '[' Value ']'

Condition: BOOLEAN;

Exp	: AddSub
	| Value
	| '(' Exp ')' { $$=$2; };

Step: Name INCREMENT { $$=tree("++", 1131, $2, 1, $1); }
	| Name DECREMENT { $$=tree("--", 1131, $2, 1, $1); };

RelationOp: LESS_EQUAL | GREATER_EQUAL | '>' | '<';
Relation: AddSub | Relation RelationOp AddSub;
Assigment	: Name '=' Exp
			| FieldAccess '=' Exp;

AddSub: MultDiv | AddSub '+' MultDiv | AddSub '-' MultDiv;
MultDiv: UnaryExp | MultDiv '*' UnaryExp | MultDiv '/' UnaryExp;
UnaryExp: '-' UnaryExp | '!' UnaryExp ;
