%{
	#include "main.h"
	#include "token.h"
%}

%union{
	struct token *token;
	int integer;
}

// Operators
%token <token> INCREMENT DECREMENT AND NOT OR
// Comparators
%token <token> EQUALS NOT_EQUAL LESS_EQUAL GREATER_EQUAL
// Types
%token <token> BOOLEAN CHAR DOUBLE INT
// Reserved words
%token <token> BREAK CASE CLASS CONTINUE DEFAULT ELSE FOR IF INSTANCEOF NEW PUBLIC RETURN STATIC SWITCH VOID WHILE LITERAL_NULL
// Literals
%token <token> LITERAL_BOOL LITERAL_CHAR LITERAL_DOUBLE LITERAL_INT LITERAL_STRING
%token <token> ID

%%
Program: Create | Create Program;
Create: INT Name '=' LITERAL_INT ';';

Name: ID { printf("%s\n", ctoken->text); }
	| QualifiedName { printf("%s\n", ctoken->text); };
QualifiedName: Name '.' ID;
