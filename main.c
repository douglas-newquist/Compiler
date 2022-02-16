/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "jzero.tab.h"
#include "token.h"
#include "errors.h"
#include "tree.h"

/*
	Reads the current file stored in yyin
*/
void scan_tree_yyin()
{
	line = 1, column = 1;

	if (yyparse() != 0)
		error(SYNTAX_ERROR, "Invalid syntax");

	print_node(program, 0);
}

/*
	Opens and reads from the given filename
*/
void scan_tree_file(char *filename)
{
	current_file = filename;
	yyin = fopen(filename, "r");

	if (yyin == 0)
	{
		printf("The file '%s' does not exist\n", filename);
		exit(LEX_ERROR);
	}

	scan_tree_yyin();
	fclose(yyin);
}

/*
	Gets the file extension of the given filename
*/
char *extension(char *filename)
{
	char *i = strrchr(filename, '.');

	if (i == NULL)
		return "";

	return i;
}

/*
	Adds the .java extension to a file without one, otherwise leaves it alone
*/
char *fix_extension(char *filename)
{
	char *ext = extension(filename);

	if (strcmp(extension(filename), ".java") == 0)
		return filename;

	if (strcmp(ext, "") == 0)
		return strcat(filename, ".java");

	printf("%s is not a .java file\n", filename);
	exit(LEX_ERROR);
}

int main(int argc, char const *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		char *filename = fix_extension((char *)argv[i]);

		scan_tree_file(filename);

		free_node(program, TRUE);
		free_tokens(tokens);
	}

	if (argc == 1)
	{
		yyin = stdin;
		current_file = "stdin";
		scan_tree_yyin();

		free_node(program, TRUE);
		free_tokens(tokens);
	}

	return 0;
}

void newline()
{
	column = 1;
	line++;
}

void comment()
{
	size_t size = strlen(yytext);
	for (size_t i = 0; i < size; i++)
	{
		if (yytext[i] == '\n')
			newline();
		else
			column++;
	}
}

void whitespace()
{
	column += strlen(yytext);
}

int token(int category)
{
	ctoken = create_token(category);
	tokens = add(tokens, ctoken);
	column += strlen(yytext);
	return category;
}

void error(int code, char *message)
{
	fprintf(stderr, "Error in %s:%d:%d\n%s: %s\n",
			current_file,
			line, column,
			message,
			yytext);
	exit(code);
}

void yyerror(char *message) { error(LEX_ERROR, message); }

void pattern(char *message)
{
	printf("%s\n", message);
}
