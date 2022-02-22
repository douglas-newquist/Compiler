/*
	Douglas Newquist
*/

/**
 * @brief Parses the given string as an integer
 */
int parse_int(char *text);

/**
 * @brief Parses the given string as an double
 */
double parse_double(char *text);

/**
 * @brief Parses true/false as an integer 1/0
 */
int parse_bool(char *text);

/**
 * @brief Parses and de-escapes the given string
 */
char *parse_string(char *text);

/**
 * @brief Parses a character from the given string
 */
char parse_char(char *text);
