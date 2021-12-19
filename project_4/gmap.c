#include "gmap.h"
#include "entry.h"
#include"string_key.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

char* gmap_error = "ERROR!!!";
#define GMAP_INITIAL_CAPACITY 100
void embiggen(gmap* m);

gmap* gmap_create(void* (*cp)(const void*), int (*comp)(const void*, const void*), size_t(*h)(const void* s), void (*f)(void*))
{
    if (h == NULL || cp == NULL || comp == NULL || f == NULL)
    {
        // one of the required functions was missing
        return NULL;
    }
    gmap* result = malloc(sizeof(gmap));
    if (result != NULL)
    {
        // remember the functions used to manipulate the keys
        result->copy = cp;
        result->compare = comp;
        result->hash = h;
        result->free = f;
        // initialize the table
        result->table = malloc(sizeof(gmap_node*) * GMAP_INITIAL_CAPACITY);
        result->capacity = (result->table != NULL ? GMAP_INITIAL_CAPACITY : 0);
        result->size = 0;
        for (size_t i = 0; i < result->capacity; i++)
        {
            result->table[i] = NULL;
        }
    }
    return result;
}

size_t gmap_size(const gmap* m)
{
    if (m == NULL)
    {
        return 0;
    }
    return m->size;
    //get the size
}

void* gmap_put(gmap* m, const void* key, void* value)
{
    if (m == NULL || key == NULL || value == NULL)
    {
        return gmap_error;
    }
    if (m->size >= m->capacity)
    {
        embiggen(m);
    }
    //If the size is too small, make it larger
    int index = m->hash(key) % (m->capacity);
    gmap_node* curr = m->table[index];//Find the position of the chain
    int* result;
    while (curr != NULL)
    {
        if (strcmp(curr->node_id, key) == 0)
        {
            result = curr->node_dist;
            curr->node_dist = value;
            return result;
        }
        //Find the possible exist node inside and update it
        curr = curr->next;
    }
    char* anotherkey = duplicate(key);
    gmap_node* anothernode = malloc(sizeof(gmap_node));
    anothernode->values = malloc(sizeof(node_value));
    anothernode->node_id = anotherkey;
    anothernode->node_dist = value;
    anothernode->values->matches = 0;
    anothernode->values->score = 0;
    anothernode->values->winning = 0;
    anothernode->next = m->table[index];
    m->table[index] = anothernode;
    //make up another node and stuff it inside the map
    m->size++;
    return NULL;
}

void* gmap_remove(gmap* m, const void* key)
{
    if (m == NULL || key == NULL)
    {
        return NULL;
    }
    int index = m->hash(key) % (m->capacity);
    gmap_node* curr;
    gmap_node* curr2;
    int* result;
    curr = m->table[index];
    while (curr != NULL)
    {
        if (curr->next == NULL)
        {
            if (strcmp(curr->node_id, key) == 0)
            {
                result = curr->node_dist;
                m->table[index] = NULL;
                free(curr->node_id);
                free(curr->values);
                free(curr);
                m->size--;
                return result;//current key is matched and is the last one
            }
            else
            {
                return NULL;
            }
        }
        if (strcmp(m->table[index]->node_id, key) == 0)
        {
            result = m->table[index]->node_dist;
            m->table[index] = curr->next;
            free(curr->node_id);
            free(curr->values);
            free(curr);
            m->size--;
            return result;
            //the one matched is the first key
        }
        if (strcmp(curr->next->node_id, key) == 0)
        {
            curr2 = curr->next;
            result = curr2->node_dist;
            curr->next = curr2->next;
            free(curr2->node_id);
            free(curr2->values);
            free(curr2);
            m->size--;
            return result;
            //matched one is in the middle
        }
        curr = curr->next;
    }
    return NULL;
}

bool gmap_contains_key(const gmap* m, const void* key)
{
    if (m == NULL || key == NULL)
    {
        return false;
    }
    int index = m->hash(key) % (m->capacity);
    gmap_node* curr = m->table[index];
    while (curr != NULL)
    {
        if (strcmp(curr->node_id, key) == 0)
        {
            return true;
        }
        curr = curr->next;
    }
    //search for every node in the map
    return false;
}


void* gmap_get(gmap* m, const void* key)
{
    if (m == NULL || key == NULL)
    {
        return gmap_error;
    }
    int index = m->hash(key) % (m->capacity);
    gmap_node* curr = m->table[index];
    int* result;
    while (curr != NULL)
    {
        if (strcmp(curr->node_id, key) == 0)
        {
            result = curr->node_dist;
            return result;
        }
        curr = curr->next;
    }
    return NULL;
    //get the desired distribution or return NULL
}

void gmap_for_each(gmap* m, void (*f)(const void*, void*, void*), void* arg)
{
    if (m == NULL || f == NULL || arg == NULL)
    {
        return;
    }
    for (int i = 0; i < m->capacity; i++)
    {
        gmap_node* curr = m->table[i];
        while (curr != NULL)
        {
            f(curr->node_id, curr->node_dist, arg);
            curr = curr->next;
        }
    }
}

const void** gmap_keys(gmap* m)
{
    if (m == NULL)
    {
        return NULL;
    }
    int totalsize = gmap_size(m);
    const void** arr = malloc(totalsize * sizeof(char*));
    int index = 0;
    for (int i = 0; i < m->capacity; i++)
    {
        gmap_node* curr = m->table[i];
        while (curr != NULL)
        {
            arr[index] = curr->node_id;
            index++;
            curr = curr->next;
        }
    }
    return arr;
}
//checked
gmap_node** all_gmap_nodes(gmap* m)//LIKE gmap_keys, get all the nodes of a map in an array
{
    if (m == NULL)
    {
        return NULL;
    }
    int totalsize = gmap_size(m);
    gmap_node** arr = malloc(totalsize * sizeof(gmap_node*));
    int index = 0;
    for (int i = 0; i < m->capacity; i++)
    {
        gmap_node* curr = m->table[i];
        while (curr != NULL)
        {
            arr[index] = curr;
            index++;
            curr = curr->next;
        }
    }
    return arr;
}

void embiggen(gmap* m)//double the map capacity and rehash/redistribute all the nodes
{
    if (m == NULL)
    {
        return;
    }
    int origsize = m->capacity;
    gmap_node** extract = all_gmap_nodes(m);
    if (extract == NULL)
    {
        return;
    }
    m->table = realloc(m->table, sizeof(gmap_node) * 2 * origsize);
    m->capacity = 2 * m->capacity;
    for (int k = 0; k < m->capacity; k++)
    {
        m->table[k] = NULL;
    }
    //To make the new table
    for (int l = 0; l < m->size; l++)
    {
        int index = m->hash(extract[l]->node_id) % (m->capacity);
        if (m->table[index] == NULL)
        {
            m->table[index] = extract[l];
            extract[l]->next = NULL;
        }
        else
        {
            extract[l]->next = m->table[index];
            m->table[index] = extract[l];
        }
    }
    //redistribute all entries in the original map into the embiggened new map
    free(extract);
}

void gmap_destroy(gmap* m)
{
    if (m == NULL)
    {
        return;
    }
    for (int i = 0; i < m->capacity; i++)
    {
        gmap_node* curr1 = m->table[i];
        gmap_node* curr2;
        while (curr1 != NULL)
        {
            curr2 = curr1->next;
            free(curr1->node_id);
            free(curr1->values);
            free(curr1);
            if (curr2 == NULL)
            {
                break;
            }
            curr1 = curr2;
            curr2 = curr2->next;
        }
    }
    free(m->table);
    free(m);
}

gmap_node* gmap_another_get(gmap* m, const void* key)
{
    if (m == NULL || key == NULL);
    {
        return NULL;
    }
    int index = m->hash(key) % (m->capacity);
    gmap_node* curr = m->table[index];
    while (curr != NULL)
    {
        if (strcmp(curr->node_id, key) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
    //cases to return error!!!
}
//checked

void update_score(const void* key1, const void* key2, gmap* m, int total_battle, int* battlevalue)
{
    //Update the score for every match key1 and key2
    double score1 = 0;
    double score2 = 0;
    double halfvalue;
    if (!gmap_contains_key(m, key1) || !gmap_contains_key(m, key2))
        return;
    //if one of them is not present in the map,end the funtion
    gmap_node* keyA = gmap_another_get(m, key1);
    gmap_node* keyB = gmap_another_get(m, key2);
    //get the nodes where key1 and key2 lie
    printf("what3??\n");
    keyA->values->matches++;
    printf("what4??\n");
    keyB->values->matches++;
    int* keyAdist = keyA->node_dist;
    int* keyBdist = keyB->node_dist;
    for (int i = 0; i < total_battle; i++)
    {
        if (keyAdist[i] > keyBdist[i])
        {
            score1 = score1 + battlevalue[i];
            continue;
        }
        if (keyAdist[i] == keyBdist[i])
        {
            halfvalue = battlevalue[i];
            score1 = score1 + halfvalue / 2;
            score2 = score2 + halfvalue / 2;
            continue;
        }
        if (keyAdist[i] < keyBdist[i])
        {
            score2 = score2 + battlevalue[i];
            continue;
        }
    }
    keyA->values->score = keyA->values->score + score1;
    keyB->values->score = keyB->values->score + score2;
    //update the score and match
    if (score1 > score2)
    {
        keyA->values->winning++;
    }
    if (score1 < score2)
    {
        keyB->values->winning++;
    }
    if (score1 == score2)
    {
        keyA->values->winning = keyA->values->winning + 0.5;
        keyB->values->winning = keyB->values->winning + 0.5;

    }
    //update the winner
}