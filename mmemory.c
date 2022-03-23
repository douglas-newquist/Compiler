/*
	Douglas Newquist

	Automates freeing blocks of memory allocated
*/

#include "mmemory.h"
#include "list.h"

#define Memory struct memory_list

Memory
{
	// Next block allocated
	Memory *next;
	// BLock allocated
	void *block;
};

Memory *head = NULL, *tail = NULL;

/**
 * @brief Allocates a block of memory
 *
 * @param size Size of block to allocate
 * @param free Function to free this block, NULL defaults to free()
 * @return void* Block allocated
 */
void *alloc(size_t size)
{
	Memory *element = malloc(sizeof(Memory));
	element->block = malloc(size);
	element->next = NULL;

	if (head == NULL)
		head = element;
	else
		tail->next = element;

	tail = element;

	return element->block;
}

/**
 * @brief Frees everything from memory
 */
void free_memory()
{
	Memory *next;

	while (head != NULL)
	{
		next = head->next;

		free(head->block);
		free(head);

		head = next;
	}

	head = NULL;
	tail = NULL;
}
