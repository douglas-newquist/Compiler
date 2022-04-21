/*
	Douglas Newquist
*/

#ifndef TOKENS_FLAG

#define NONE_FLAG (1 << 0)
#define TOKENS_FLAG (1 << 1)
#define TREE_FLAG (1 << 2)
#define BISON_FLAG (1 << 3)
#define DOT_FLAG (1 << 4)
#define SYMBOLS_FLAG (1 << 5)
#define CODE_FLAG (1 << 6)

int options, flag_count;

/**
 * @brief Gets the code for the given flag, 0 if not a flag
 */
int flag(char *arg);
int has_flags(int v, int flags);
#endif
