#include <string.h>

#include "string_key.h"

size_t string_hash(const void* key)
{
    const char* s = key;
    size_t sum = 0;
    size_t factor = 29;
    while (s != NULL && *s != '\0')
    {
        sum += *s * factor;
        s++;
        factor *= 29;
    }

    return sum;
}

void* string_copy(const void* key)
{
    char* s = malloc(strlen(key) + 1);
    if (s != NULL)
    {
        strcpy(s, key);
    }
    return s;
}

int string_compare(const void* key1, const void* key2)
{
    return strcmp(key1, key2);
}

int string_ptr_compare(const void* key1, const void* key2)
{
    return strcmp(*(char**)key1, *(char**)key2);
}