%{
	#include "main.h"
	#include "token.h"
%}
// Operators
%token INCREMENT DECREMENT AND NOT OR
// Comparators
%token EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL
// Types
%token BOOLEAN CHAR DOUBLE INT
// Reserved words
%token BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF INSTANCEOF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE LITERAL_NULL
// Literals
%token LITERAL_BOOL LITERAL_CHAR LITERAL_DOUBLE LITERAL_INT LITERAL_STRING
%token ID

%union{
	struct token *token;
	int integer;
}

%type <token> Visability
%type <integer> int
%%
Program: Class

Name: ID | QualifiedName { pattern("Found a name"); };
QualifiedName: Name '.' ID;

AnyType: VOID | Type;
Type: SingleType | ListType;
ListType: SingleType '[' ']' | ListType '[' ']';
SingleType: FixedType | Name;
FixedType: int | DOUBLE | BOOLEAN | CHAR;
int: INT { printf("%d\n", ctoken->ival); };

Literal: LITERAL_INT | LITERAL_BOOL | LITERAL_STRING | LITERAL_CHAR | LITERAL_DOUBLE | LITERAL_NULL;
Value: Literal | '(' Exp ')' | FieldAccess;
FieldAccess: Value '.' ID;

Visability: PUBLIC;

ArgsDef: ArgDef | ArgsDef ',' ArgDef | ;
ArgDef: Type ID;
Args: Arg | Args ',' Arg;
Arg: Value;

// public class name { ... }
Class: Visability  CLASS ID ClassBody;
ClassBody: '{' ClassBodyDecls '}' | '{' '}';
ClassBodyDecls: ClassBodyDecl | ClassBodyDecls ClassBodyDecl;
ClassBodyDecl: Field | Method;

// type something = value;
Field: Type VarsDef ';';
VarsDef: VarDef | VarsDef ',' VarDef;
VarDef: ID | VarDef '[' ']';


// public static type name(args) { ... }
Method: MethodHead Block;
MethodHead: Visability STATIC AnyType ID '(' ArgsDef ')';


Block: '{' Statements '}' | '{' '}';


Statements: Statement | Statements Statement;
Statement: ';' | Block | If | IfElse | IfElseIf | IfElseIfElse | While | For | Break | Return | QualifiedName '(' Literal ')' ';';

// if (condition) { ... } else if (condition) { ... } ... else { ... }
IfElseIfElse: IfElseIf ELSE Block;
// if (condition) { ... } else if (condition) { ... } ...
IfElseIf: If ELSE If | IfElseIf ELSE If;
// if (condition) { ... } else { ... }
IfElse: If ELSE Block;
// if (condition) { ... }
If: IF '(' Condition ')' Block

// while (conditon) { ... }
While: WHILE '(' Condition ')' Block;

Break: BREAK ';';
Return: RETURN Exp ';' | RETURN ';';
SwitchCase: CASE Value ':' Statements Break ';';
Continue: CONTINUE ';';
Default: DEFAULT ':';

For: FOR '(' ForInit ';' ForCond ';' ForInc  ')' Block
ForInit: VarDef | ;
ForCond: Condition | ;
ForInc: Statement | ;

Condition: BOOLEAN;

Exp: AddSub;

AssignmentOp: '=' | INCREMENT | DECREMENT;
RelationOp: LESS_EQUAL | GREATER_EQUAL | '>' | '<';
Relation: AddSub | Relation RelationOp AddSub;
Assigment: Name '=' Exp | FieldAccess '=' Exp;

AddSub: MultDiv | AddSub '+' MultDiv | AddSub '-' MultDiv;
MultDiv: UnaryExp | MultDiv '*' UnaryExp | MultDiv '/' UnaryExp;
UnaryExp: '-' UnaryExp | '!' UnaryExp ;
