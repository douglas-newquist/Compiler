/*
	Douglas Newquist
*/

#ifndef HashTable

#define HashTable struct hashtable

#include "list.h"

HashTable
{
	// Number of elements in the hash table
	int count;
	int bucket_count;
	List **buckets;
	// Hashing function
	int (*hasher)(void *);
	// Comparision function
	int (*equals)(void *, void *);
};

#define foreach_hashtable(name, hash)                           \
	for (int hash_i = 0; hash_i < hash->bucket_count; hash_i++) \
	foreach_list(name, hash->buckets[hash_i])

/**
 * @brief Create a new hash table
 *
 * @param buckets The number of buckets
 * @param hasher Function to hash an element with
 * @param equals Function to compare two elements
 */
HashTable *create_hashtable(int buckets, int (*hasher)(void *), int (*equals)(void *, void *));

/**
 * @brief Adds an element to the given hash table
 */
void hashtable_add(HashTable *hashtable, void *element);

/**
 * @brief Checks if the given hash table contains an element
 *
 * @param element Element to search for
 * @return void* The value in the the hash table
 */
void *hashtable_contains(HashTable *hashtable, void *element);
#endif
