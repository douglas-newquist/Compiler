/*
	Douglas Newquist
*/

#ifndef _MEMORY
#define _MEMORY

#include <stdlib.h>

/**
 * @brief Allocates a block of memory
 *
 * @param size Size of block to allocate
 * @param free Function to free this block, NULL defaults to free()
 * @return void* Block allocated
 */
void *alloc(size_t size);

/**
 * @brief Frees everything from memory
 */
void free_memory();

#endif
