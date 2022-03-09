/*
	Douglas Newquist
*/

#include <stdlib.h>
#include "hash.h"
#include "main.h"

/**
 * @brief Create a new hash table
 *
 * @param buckets The number of buckets
 * @param hasher Function to hash an element with
 * @param equals Function to compare two elements
 */
HashTable *create_hashtable(int buckets, int (*hasher)(void *), int (*equals)(void *, void *))
{
	HashTable *hash = (HashTable *)malloc(sizeof(HashTable));
	hash->count = 0;
	hash->bucket_count = buckets;
	hash->hasher = hasher;
	hash->equals = equals;
	hash->buckets = (List **)malloc(sizeof(List *) * buckets);

	for (int i = 0; i < buckets; i++)
		hash->buckets[i] = NULL;

	return hash;
}

/**
 * @brief Frees the given hash table
 *
 * @param list
 * @param freer Function to free an individual element
 */
void free_hashtable(HashTable *hashtable, void (*freer)(void *))
{
	if (hashtable == NULL)
		return;

	for (int i = 0; i < hashtable->bucket_count; i++)
		free_list(hashtable->buckets[i], freer);

	free(hashtable->buckets);
	free(hashtable);
}

/**
 * @brief Adds an element to the given hash table
 */
void hashtable_add(HashTable *hashtable, void *element)
{
	if (hashtable == NULL || hashtable_contains(hashtable, element))
		return;

	int i = hashtable->hasher(element) % hashtable->bucket_count;

	hashtable->buckets[i] = list_add(hashtable->buckets[i], element);
	hashtable->count++;
}

/**
 * @brief Checks if the given hash table contains an element
 *
 * @param element Element to search for
 * @return void* The value in the the hash table
 */
void *hashtable_contains(HashTable *hashtable, void *element)
{
	if (hashtable == NULL)
		return NULL;

	int i = hashtable->hasher(element) % hashtable->bucket_count;
	List *bucket = hashtable->buckets[i];

	foreach_list(element2, bucket)
	{
		if (hashtable->equals(element, element2->value))
			return element2->value;
	}

	return NULL;
}
