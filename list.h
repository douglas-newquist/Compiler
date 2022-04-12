/*
	Douglas Newquist
*/

#ifndef List

#define List struct linkedlist
#define ListElement struct listelement

#define foreach_list(name, list) \
	if (list != NULL)            \
		for (ListElement *name = list->head; name != NULL; name = name->next)

#define LIST_ADD(list, element) list = list_add(list, element)

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

/**
 * @brief Appends list 2 the end of list 1
 */
List *list_merge(List *l1, List *l2);
#endif
