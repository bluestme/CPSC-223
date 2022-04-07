#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "string_key.h"
#include "pqueue.h"
#define GMAP_INITIAL_CAPACITY 100

void embiggen_index(undigraph* g);//embiggen the has table when size=capcity
bool undigraph_add_vertex(undigraph* g, char* name); // add a new vertex into graph
gfnode* get_vertex(undigraph* g, char* name); //get the pointer pointing to the node with the same name

//=============================================================Public Functions========================================================

undigraph* undigraph_create()
{
    undigraph* result = malloc(sizeof(undigraph));
    if (result != NULL)
    {
        result->map = malloc(sizeof(gfnode*) * GMAP_INITIAL_CAPACITY);
        result->capacity = (result->map != NULL ? GMAP_INITIAL_CAPACITY : 0);
        result->size = 0;
        for (size_t i = 0; i < result->capacity; i++)
        {
            result->map[i] = NULL;
        }
    }//initialize the graph hash table with original capacity and size 0
    return result;
}


void undigraph_destroy(undigraph* g)
{
    if (g == NULL)
    {
        return;
    }
    for (int i = 0; i < g->capacity; i++)
    {
        gfnode* curr1 = g->map[i];
        gfnode* curr2;
        gfnode* curr3;
        while (curr1 != NULL)
        {
            curr2 = curr1->degree;
            while (curr2 != NULL)
            {
                curr3 = curr2->degree;
                free(curr2->name);
                free(curr2);
                curr2 = curr3;
            }
            curr3 = curr1;
            curr1 = curr1->next;
            free(curr3->name);
            free(curr3);
        }
        //for each node in has table,free all its degrees as well as their names
    }
    free(g->map);
    free(g);
}

bool undigraph_has_vertex(undigraph* g, char* name)
{
    if (g == NULL || name == NULL)
    {
        return false;
    }
    int index = string_hash(name) % (g->capacity);
    gfnode* curr = g->map[index];
    while (curr != NULL)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return true;
        }
        curr = curr->next;
    }
    //search for every node in the map (but avoided degrees)
    return false;
}


bool undigraph_has_edge(undigraph* g, char* a, char* b)
{
    if (g == NULL || a == NULL || b == NULL)
    {
        return false;
    }
    int index = string_hash(a) % (g->capacity);
    gfnode* curr = g->map[index];
    gfnode* curr0;
    while (curr != NULL)
    {
        if (strcmp(curr->name, a) == 0)
        {
            curr0 = curr->degree;
            while (curr0 != NULL)
            {
                if (strcmp(curr0->name, b) == 0)
                {
                    return true;
                }
                curr0 = curr0->degree;
            }
            return false;
        }
        curr = curr->next;
    }
    //search for every node in the map, if the node exists, search for its degree to see if there are names of the other query
    return false;
}

bool undigraph_add_edge(undigraph* g, char* a, char* b)
{
    if (g == NULL || a == NULL || b == NULL || a[0] == '\0' || b[0] == '\0')
    {
        return false;
    }
    if (undigraph_has_edge(g, a, b))
    {
        return false;
    }
    //if input is null, or the edge exists
    gfnode* anothernode1 = malloc(sizeof(gfnode));
    anothernode1->name = string_copy(a);
    anothernode1->next = NULL;
    anothernode1->degree = NULL;
    gfnode* anothernode2 = malloc(sizeof(gfnode));
    anothernode2->name = string_copy(b);
    anothernode2->next = NULL;
    anothernode2->degree = NULL;
    //create 2 degree nodes for each other
    if (undigraph_has_vertex(g, a))
    {
        anothernode1->location = get_vertex(g, a)->location;
    }
    if (!undigraph_has_vertex(g, a))
    {
        undigraph_add_vertex(g, a);
        anothernode1->location = g->size - 1;
    }
    if (undigraph_has_vertex(g, b))
    {
        anothernode2->location = get_vertex(g, b)->location;
    }
    if (!undigraph_has_vertex(g, b))
    {
        undigraph_add_vertex(g, b);
        anothernode2->location = g->size - 1;
    }
    //if the vertexes exists, get them, if not, add one into the map
    gfnode* curr1 = get_vertex(g, a);
    gfnode* curr2 = get_vertex(g, b);
    anothernode2->degree = curr1->degree;
    curr1->degree = anothernode2;
    anothernode1->degree = curr2->degree;
    curr2->degree = anothernode1;
    //put the degress of nodes
    return true;
}

size_t undigraph_bfs_distance(undigraph* g, char* start, char* target)
{
    int mapsize = g->size;
    int* color = malloc(sizeof(int) * mapsize);
    int* d = malloc(sizeof(int) * mapsize);//The shortest distance from the start
    int* prev = malloc(sizeof(int) * mapsize);//The previous node on the path with the shortest distance from the start
    for (int i = 0; i < mapsize; i++)
    {
        color[i] = 0; //0 means unseen
        d[i] = 0;
        prev[i] = -1;
    }
    gqueue* ref = gqueue_create();
    gfnode* curr = get_vertex(g, start);
    color[curr->location] = 1;//1 means in queue
    gqueue_enqueue(ref, curr);//enqueue the start node
    while (is_gqueue_empty(ref) == false)
    {
        gfnode* curr0 = gqueue_dequeue(ref);//dequeue a node
        gfnode* currd = curr0->degree;
        while (currd != NULL)//search for all the degrees of the dequeued node and MAP BACK to the original graph, update them one by one and enqueue them if they are unseen
        {
            if (color[currd->location] == 0)
            {
                gfnode* currdd = get_vertex(g, currd->name);
                gqueue_enqueue(ref, currdd);
                color[currd->location] = 1;
                prev[currd->location] = curr0->location;
                d[currd->location] = d[curr0->location] + 1;
            }
            currd = currd->degree;
        }
        color[curr0->location] = 2;//means done
    }
    size_t final = d[get_vertex(g, target)->location];
    gqueue_destroy(ref);
    free(color);
    free(d);
    free(prev);
    return final;
}
char** undigraph_common_neighbors(undigraph* g, char* a, char* b, size_t* count, bool* is_valid_query)
{
    *count = 0;
    int arrindex = 0;
    int size = g->size;
    int tablesize = 0;
    gfnode* curr1 = get_vertex(g, a);
    gfnode* curr0 = curr1->degree;
    while (curr0 != NULL)
    {
        tablesize++;
        curr0 = curr0->degree;
    }
    char** arr = malloc(sizeof(*arr) * tablesize);
    //===================================================Get the degree of vertex a============================================
    tablesize = tablesize * 2;
    if (tablesize == 0)
    {
        return NULL;
    }
    gfnode** anothermap = malloc(sizeof(gfnode*) * tablesize);
    for (int i = 0; i < tablesize; i++)
    {
        anothermap[i] = NULL;
    }
    curr0 = curr1->degree;
    while (curr0 != NULL)
    {
        int index10 = string_hash(curr0->name) % (tablesize);
        gfnode* curr10 = anothermap[index10];
        gfnode* anothernode = malloc(sizeof(gfnode));
        anothernode->name = string_copy(curr0->name);
        anothernode->degree = NULL;
        anothernode->next = anothermap[index10];
        anothermap[index10] = anothernode;
        curr0 = curr0->degree;
    }
    //===================================================Put degrees of a in another hash table with capacity twice as its number============================================
    gfnode* curr2 = get_vertex(g, b);
    curr0 = curr2->degree;
    while (curr0 != NULL)
    {
        int index20 = string_hash(curr0->name) % (tablesize);
        gfnode* curr20 = anothermap[index20];
        while (curr20 != NULL)
        {
            if ((strcmp(curr20->name, curr0->name) == 0) && (strcmp(a, curr0->name) != 0))
            {
                char* cpname = string_copy(curr0->name);
                arr[arrindex] = cpname;
                *count = *count + 1;
                arrindex = arrindex + 1;
                break;
            }
            curr20 = curr20->next;
        }
        curr0 = curr0->degree;
    }
    //========================================================find overlap in degrees of B in the hash table======================================================
    for (int j = 0; j < tablesize; j++)
    {
        gfnode* curr1 = anothermap[j];
        gfnode* curr2;
        while (curr1 != NULL)
        {
            curr2 = curr1->next;
            free(curr1->name);
            free(curr1);
            if (curr2 == NULL)
            {
                break;
            }
            curr1 = curr2;
        }
    }
    free(anothermap);
    //=====================================================Destroy the helper hash table======================================================
    return arr;
}
//=============================================================Helper Functions========================================================

bool undigraph_add_vertex(undigraph* g, char* name)
{
    if (g == NULL || name == NULL)
    {
        return false;
    }
    if (g->size >= g->capacity)
    {
        embiggen_index(g);
    }
    int index = string_hash(name) % (g->capacity);
    gfnode* curr = g->map[index];
    while (curr != NULL)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return false;
        }
        curr = curr->next;
    }
    //search for every node in the map to see if there no such vertex
    gfnode* anothernode = malloc(sizeof(gfnode));
    anothernode->name = string_copy(name);
    anothernode->degree = NULL;
    anothernode->next = g->map[index];
    anothernode->location = g->size;
    g->map[index] = anothernode;
    //make up another node and stuff it inside the graph
    g->size++;
    return true;
}

void embiggen_index(undigraph* g)//double the map capacity and rehash/redistribute all the nodes
{
    if (g == NULL)
    {
        return;
    }
    int origcapacity = g->capacity;
    int origsize = g->size;
    gfnode** extract = malloc(origsize * sizeof(gfnode*));
    int l = 0;
    for (int i = 0; i < origcapacity; i++)
    {
        gfnode* curr = g->map[i];
        while (curr != NULL)
        {
            extract[l] = curr;
            l++;
            curr = curr->next;
        }
    }//put all the previous nodes in an array
    if (extract == NULL)
    {
        return;
    }
    g->map = realloc(g->map, sizeof(gfnode) * 2 * origcapacity);
    g->capacity = 2 * g->capacity;
    for (int k = 0; k < g->capacity; k++)
    {
        g->map[k] = NULL;
    }
    //To make the new table
    for (int u = 0; u < origsize; u++)
    {
        int index = string_hash(extract[u]->name) % (g->capacity);
        if (g->map[index] == NULL)
        {
            g->map[index] = extract[u];
            extract[u]->next = NULL;
        }
        else
        {
            extract[u]->next = g->map[index];
            g->map[index] = extract[u];
        }
    }
    //redistribute all entries in the original graph into the embiggened new graph
    free(extract);
}

gfnode* get_vertex(undigraph* g, char* name)
{
    if (g == NULL || name == NULL)
    {
        return false;
    }
    int index = string_hash(name) % (g->capacity);
    gfnode* curr = g->map[index];
    while (curr != NULL)
    {
        if (strcmp(curr->name, name) == 0)
        {
            return curr;
            //search for the name in the table and return the node
        }
        curr = curr->next;
    }
    return NULL;
    //if nothing found, return NULL
}
