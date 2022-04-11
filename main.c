/*
	Douglas Newquist
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "flags.h"
#include "imcode.h"
#include "jzero.tab.h"
#include "main.h"
#include "mmemory.h"
#include "symbol.h"
#include "symboltable.h"
#include "token.h"
#include "tree.h"

#ifdef DEBUG
#include <signal.h>
int in_panic = FALSE;

/**
 * @brief Catches a segmentation fault and prints out an error then continues exit
 */
void panic(int signal)
{
	if (in_panic)
		// Problem ocurred inside panic
		exit(signal);
	in_panic = TRUE;

	switch (signal)
	{
	case SIGSEGV:
		error(SIGSEGV, "!!! Segmentation Fault !!!");
	}

	exit(-1);
}
#endif

void free_all()
{
	free_memory();

	if (yyin && yyin != stdin)
	{
		// Close file if not closed for some reason
		fclose(yyin);
		yyin = NULL;
	}

	yylex_destroy();
}

/**
 * @brief Do stuff after the program has been parsed
 */
void post_read()
{
	if (has_flags(options, TOKENS_FLAG))
		print_tokens(tokens);

	if (has_flags(options, DOT_FLAG | TREE_FLAG))
		print_dot_tree(program, 0);
	else if (has_flags(options, TREE_FLAG))
		print_tree(program, 0, "| ");

	table = generate_symboltable(program);

	if (has_flags(options, DOT_FLAG | SYMBOLS_FLAG))
		print_dot_symbols(table);
	else if (has_flags(options, SYMBOLS_FLAG))
		print_symbols(table);

	List *code = generate_code(table, program);
	print_code(code);
}

/*
	Reads the current file stored in yyin
*/
void read_yyin()
{
	line = 1, column = 1;

#if DEBUG
	printf("Reading from %s\n", current_file);
#endif

	if (yyparse() != 0)
		error(SYNTAX_ERROR, "Invalid syntax");

	post_read();
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

	printf("%s is not a java file\n", filename);
	exit(LEX_ERROR);
}

void read_file(char *filename)
{
	strcpy(current_file, filename);
	fix_extension(current_file);

	yyin = fopen(current_file, "r");

	if (yyin == 0)
	{
		yytext = current_file;
		error(LEX_ERROR, "File not found");
	}

	read_yyin();

	fclose(yyin);
	yyin = NULL;
}

int main(int argc, char *argv[])
{
	options = 0;

#ifdef DEBUG
	signal(SIGSEGV, panic);
#endif

	for (int i = 1; i < argc; i++)
	{
		switch (flag(argv[i]))
		{
#if DEBUG
		case BISON_FLAG:
			yydebug = 1;
			break;
#endif
		}
	}

	int files_read = 0;

	for (int i = 1; i < argc; i++)
	{
		if (flag(argv[i]) == 0)
		{
			read_file(argv[i]);
			files_read++;
		}
	}

	if (files_read == 0)
	{
		yyin = stdin;
		strcpy(current_file, "stdin");
		read_yyin();
	}

	free_all();

	return 0;
}

/**
 * @brief Marks the current token as a newline
 */
void newline()
{
	column = 1;
	line++;
}

/**
 * @brief Marks the current token as a comment
 */
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

/**
 * @brief Marks the current toke as whitespace
 */
void whitespace()
{
	column += strlen(yytext);
}

int token(int category)
{
	yylval.tree = tree_token(create_token(category));
	column += strlen(yytext);
	return category;
}

void set_pos(Token *token)
{
	strcpy(current_file, token->filename);
	yytext = token->text;
	line = token->line;
	column = token->column;
}
