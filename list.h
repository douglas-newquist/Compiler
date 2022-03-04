/*
	Douglas Newquist
*/

#ifndef List
#define List struct linkedlist
#define ListElement struct listelement

#define foreach_list(list) for (ListElement *element = list->head; element != NULL; element = element->next)

ListElement
{
	void *value;
	ListElement *next;
};

List
{
	int size;
	ListElement *head, *tail;
};

/**
 * @brief Adds an element to the end of the list
 *
 * @param list Linked list to append to
 * @param value Element being added
 */
List *
list_add(List *list, void *value);

/**
 * @brief Frees the given linked list
 *
 * @param list
 * @param freer Function to free an individual element
 */
List *free_list(List *list, void (*freer)(void *));

void print_list(List *list, void (*printer)(void *));

#endif
