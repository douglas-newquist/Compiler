/*
	Douglas Newquist
*/

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

List *list_tail(List *list)
{
	if (list == NULL)
		return NULL;

	while (list->next != NULL)
		list = list->next;

	return list;
}

List *list_add(List *list, void *value)
{
	List *current = (List *)malloc(sizeof(List));
	current->value = value;
	current->next = NULL;

	if (list != NULL)
	{
		list_tail(list)->next = current;
		return list;
	}

	return current;
}

List *free_list(List *list, void (*freer)(void *))
{
	while (list != NULL)
	{
		List *next = list->next;

		if (freer != NULL)
			(*freer)(list->value);

		free(list);
		list = next;
	}

	return NULL;
}
