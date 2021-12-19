#ifndef __STRING_KEY_H__
#define __STRING_KEY_H__

#include <stdlib.h>
#include "graph.h"
typedef struct _graphnode gfnode;
struct undigraph
{
	int size;
	int capacity;
	gfnode** map;
};

struct _graphnode
{
	gfnode* next;
	gfnode* degree;
	char* name;
	int location;
};

/**
 * A hash function for strings.
 *
 * @param key a pointer to a string, non-NULL
 * @return the hash code of the string
 */
size_t string_hash(const void* key);

/**
 * Makes a copy of the given string.  Returns NULL if there is an allocation
 * error for the copy.  It is the caller's responsibility to free the returned
 * string if it is non-NULL.
 *
 * @param key a pointer to a string, non-NULL
 * @return a pointer to a new copy of the string, or NULL
 */
void* string_copy(const void* key);

/**
 * Compares the two strings.  The return value is negative if the first
 * one comes first by a character-by-character ASCII code comparison,
 * positive if the second string comes first, and 0 if they are equal.
 *
 * @param key1 a pointer to a string, non-NULL
 * @param key2 a pointer to a string, non-NULL
 * @return an positive, negative, or zero integer according to the comparison
 */
int string_compare(const void* key1, const void* key2);

int string_ptr_compare(const void* key1, const void* key2);

#endif