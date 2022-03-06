/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

List *create_list(ListElement *element)
{
	List *list = malloc(sizeof(List));
	list->size = element == NULL ? 0 : 1;
	list->head = element;
	list->tail = element;
	return list;
}

ListElement *create_list_element(void *value)
{
	ListElement *element = malloc(sizeof(ListElement));
	element->value = value;
	element->next = NULL;
	return element;
}

List *list_add(List *list, void *value)
{
	ListElement *current = create_list_element(value);

	if (list == NULL)
		return create_list(current);

	list->size++;

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
	return list;
}

List *free_list(List *list, void (*freer)(void *))
{
	if (list == NULL)
		return NULL;

	ListElement *current = list->head, *next;

	while (current != NULL)
	{
		next = current->next;

		if (freer != NULL)
			(*freer)(current->value);

		free(current);
		current = next;
	}

	free(list);

	return NULL;
}

void print_list(List *list, void (*printer)(void *))
{
	if (list == NULL)
		return;

	ListElement *current = list->head;

	while (current != NULL)
	{
		(*printer)(current->value);
		current = current->next;
	}
}
