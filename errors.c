/*
	Douglas Newquist
*/

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"
#include "main.h"
#include "mmemory.h"

extern void free_all();

void error(int code, char *message)
{
	fprintf(stderr, "Error at %d:%d in %s\n%s: %s\n",
			line, column,
			current_file,
			message,
			yytext);

	free_all();

#if DEBUG
	printf("Exit: %d\n", code);
#endif

	exit(code);
}

void yyerror(char *message) { error(SYNTAX_ERROR, message); }

/**
 * @brief Calls error at the given token's position
 */
void error_at(Token *token, int code, char *message)
{
	if (token == NULL)
		error(code, message);

	set_pos(token);

	error(code, message);
}

char *error_message(char *pattern, ...)
{
	char *message = alloc(sizeof(char) * ERROR_BUFFER_SIZE);
	va_list args;
	va_start(args, pattern);
	vsprintf(message, pattern, args);
	va_end(args);
	return message;
}
