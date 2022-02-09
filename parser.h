/*
	Douglas Newquist
*/

int parse_int(char *text);

/*
	Parses a double value from the string
*/
double parse_double(char *text);

/*
	Parses true/false as an integer 1/0
*/
int parse_bool(char *text);

/*
	Parses and de-escapes the given string
*/
char *parse_string(char *text);

/*
	Parses a character string
*/
char parse_char(char *text);
