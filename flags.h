/*
	Douglas Newquist
*/

#ifndef TOKENS_FLAG
#define NONE_FLAG (1 << 0)
#define TOKENS_FLAG (1 << 1)
#define TREE_FLAG (1 << 2)
#define BISON_FLAG (1 << 3)
#define DOT_FLAG (1 << 4)

int options, flag_count;

int flag(char *arg);
#endif
