/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "tokens.h"
#include "main.h"

int parse_int(char *text)
{
	int a = atoi(text);

	char parsed[64];
	int b;
	sprintf(parsed, "%d", a);
	sscanf(parsed, "%d", &b);

	if (a == b)
		return a;

	error(INVALID_LITERAL, "Integer out of bounds");
	exit(-1);
}

/*
	Parses a double value from the string
*/
double parse_double(char *text)
{
	double a = atof(text);

	char parsed[64];
	double b;
	sprintf(parsed, "%lf", a);
	sscanf(parsed, "%lf", &b);

	// Check that a and b are close enough
	if (abs(a - b) <= abs(a / 100) && ((a < 0) == (b < 0)))
		return a;

	error(INVALID_LITERAL, "Float out of bounds");
	exit(-1);
}

/*
	Parses true/false as an integer 1/0
*/
int parse_bool(char *text)
{
	if (strcmp(text, "true") == 0)
		return TRUE;

	if (strcmp(text, "false") == 0)
		return FALSE;

	error(INVALID_LITERAL, "Invalid boolean value");
	exit(-1);
}

/*
	Gets the given character's escaped value
*/
char get_escaped_char(char c)
{
	switch (c)
	{
	case 'n':
		return '\n';

	case 't':
		return '\t';

	case '"':
		return '"';

	case '\'':
		return '\'';

	case 'r':
		return '\r';

	case 'f':
		return '\f';

	case '\\':
		return '\\';

	case 'a':
		return '\a';

	case 'b':
		return '\b';
	}

	error(INVALID_LITERAL, "Invalid escaped character");
	exit(-1);
}

/*
	Parses and de-escapes the given string
*/
char *parse_string(char *text)
{
	int size = strlen(text);
	char *string = (char *)malloc(sizeof(char) * size);
	// Skip first quote
	int offset = 1;

	// Zero string
	for (int i = 0; i < size; i++)
		string[i] = '\0';

	for (int i = 0; i < size - 1 - offset; i++)
	{
		// Check if this is an escape character
		if (text[i + offset] == '\\')
		{
			offset++;
			string[i] = get_escaped_char(text[i + offset]);
		}
		else
			string[i] = text[i + offset];
	}

	return string;
}

/*
	Parses a character string
*/
char parse_char(char *text)
{
	switch (strlen(text))
	{
	// Single character
	case 3:
		return text[1];

	// Escaped character
	case 4:
		return get_escaped_char(text[2]);

	default:
		error(INVALID_LITERAL, "Invalid character");
		exit(-1);
	}
}
