/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include "list.h"
#include "mmemory.h"

List *create_list()
{
	List *list = alloc(sizeof(List));
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return list;
}

ListElement *create_list_element(void *value)
{
	ListElement *element = alloc(sizeof(ListElement));
	element->value = value;
	element->next = NULL;
	return element;
}

List *list_add(List *list, void *value)
{
	ListElement *current = create_list_element(value);

	if (list == NULL)
		list = create_list();

	if (list->size == 0)
	{
		list->tail = current;
		list->head = current;
	}
	else
	{
		list->tail->next = current;
		list->tail = current;
	}

	list->size++;

	return list;
}
