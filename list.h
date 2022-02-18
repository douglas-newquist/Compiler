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

List *add(List *list, void *value);

List *tail(List *list);

/**
 * Frees the given linked list
 *
 * @param list
 * @param freer Function to free an individual element
 */
List *free_list(List *list, void (*freer)(void *));

#endif
