/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "tokens.h"
#include "token.h"

char *current_file;

/*
	Reads the current file stored in yyin
*/
Tokens *scan_yyin()
{
	line = 1;

	Tokens *tokens = NULL;

	for (int token = yylex(); token != EOF; token = yylex())
		tokens = add(tokens, create_token(token));

	return tokens;
}

/*
	Opens and reads from the given filename
*/
Tokens *scan_file(char *filename)
{
	current_file = filename;
	yyin = fopen(filename, "r");

	if (yyin == 0)
	{
		printf("The file '%s' does not exist\n", filename);
		exit(ERROR);
	}

	Tokens *result = scan_yyin();
	fclose(yyin);
	return result;
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
	exit(ERROR);
}

int main(int argc, char const *argv[])
{
	if (argc == 1)
	{
		yyin = stdin;
		current_file = "stdin";
		Tokens *tokens = scan_yyin();

		print_tokens(tokens);
		free_tokens(tokens);
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			char *filename = fix_extension((char *)argv[i]);

			Tokens *tokens = scan_file(filename);

			print_tokens(tokens);
			free_tokens(tokens);
		}
	}

	return 0;
}

void newline()
{
	line++;
}

/*
	Counts the number of times c appears in the string
*/
int count_chars(char *str, char c)
{
	int count = 0;

	for (size_t i = 0; i < strlen(str); i++)
		if (str[i] == c)
			count++;

	return count;
}

void comment()
{
	line += count_chars(yytext, '\n');
}

void whitespace() {}

void error(int code, char *message)
{
	printf("Error(%d) in %s on line %d\n%s: %s\n",
		   code,
		   current_file,
		   line,
		   message,
		   yytext);
	exit(code);
}
