/*
	Douglas Newquist
*/

struct token
{
	int category;
	char *text;
	int line;
	char *filename;

	// Integer value
	int ival;
	// Double value
	double dval;
	// String value
	char *sval;
	// Boolean value
	short bval;
	// Character value
	char cval;
};

struct tokenlist
{
	struct token *token;
	struct tokenlist *next;
};

/*
	Creates a new token with the given category
*/
struct token *create_token(int category);

/*
	Frees the given token from memory
*/
void free_token(struct token *token);

/*
	Frees the given token list from memory
*/
void free_tokens(struct tokenlist *tokens);

void print_token(struct token *token);
void print_tokens(struct tokenlist *tokens);

/*
	Adds a token to the linked list
*/
struct tokenlist *add(struct tokenlist *tokens, struct token *token);
