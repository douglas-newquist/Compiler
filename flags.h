/*
	Douglas Newquist
*/

#ifndef TOKENS_FLAG
#define TOKENS_FLAG (1 << 0)
#define TREE_FLAG (1 << 1)
#define BISON_FLAG (1 << 2)

int options, flag_count;

int flag(char *arg);
#endif
