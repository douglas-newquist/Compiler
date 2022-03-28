/*
	Douglas Newquist
*/

#ifndef List

#define List struct linkedlist
#define ListElement struct listelement

#define foreach_list(name, list) \
	if (list != NULL)            \
		for (ListElement *name = list->head; name != NULL; name = name->next)

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

List *create_list();

/**
 * @brief Adds an element to the end of the list
 *
 * @param list Linked list to append to
 * @param value Element being added
 */
List *list_add(List *list, void *value);

#endif
