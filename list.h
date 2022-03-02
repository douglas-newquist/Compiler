/*
	Douglas Newquist
*/

#ifndef List
#define List struct linkedlist

List
{
	void *value;
	List *next;
};

/**
 * @brief Adds an element to the end of the list
 *
 * @param list Linked list to append to
 * @param value Element being added
 */
List *list_add(List *list, void *value);

/**
 * @brief Gets the tail node
 */
List *list_tail(List *list);

/**
 * @brief Frees the given linked list
 *
 * @param list
 * @param freer Function to free an individual element
 */
List *free_list(List *list, void (*freer)(void *));

#endif
