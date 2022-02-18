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

void free_all()
{
	free_trees();
	free_tokens();

	if (yyin && yyin != stdin)
	{
		// Close file if not closed for some reason
		fclose(yyin);
		yyin = NULL;
	}
}

/*
	Reads the current file stored in yyin
*/
void read_yyin()
{
	line = 1, column = 1;

	if (yyparse() != 0)
		error(SYNTAX_ERROR, "Invalid syntax");
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

void read_file(char *filename)
{
	strcpy(current_file, filename);
	fix_extension(current_file);

	yyin = fopen(current_file, "r");

	if (yyin == 0)
	{
		printf("The file '%s' does not exist\n", filename);
		exit(LEX_ERROR);
	}

	read_yyin();

	fclose(yyin);
	yyin = NULL;
}

void post_read()
{
	print_tokens(tokens);
	print_tree(program, 0, "| ");
}

int main(int argc, char const *argv[])
{
	for (int i = 1; i < argc; i++)
	{
		read_file((char *)argv[i]);
		post_read();
	}

	if (argc == 1)
	{
		yyin = stdin;
		strcpy(current_file, "stdin");
		post_read();
	}

	free_all();

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
	yylval.token = create_token(category);
	yylval.tree = tree_token(yylval.token);
	// FIXME Line number broken
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

	free_all();
	exit(code);
}

void yyerror(char *message) { error(LEX_ERROR, message); }
