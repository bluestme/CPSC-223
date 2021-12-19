#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "graph.h"
#include"pqueue.h"
#include"string_key.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        return 0;
    }
    //invalid num of arguments
    FILE* input = fopen(argv[1], "r");
    if (!input)
    {
        return 0;
    }
    //cannot open file
    if (strcmp(argv[2], "distance") != 0 && strcmp(argv[2], "mutual") != 0 && strcmp(argv[2], "both") != 0)
    {
        return 0;
    }
    //second argument not valid
    undigraph* g = undigraph_create();
    char playkey1[21];
    char playkey2[21];
    while (true)
    {
        char playkey1[21];
        char playkey2[21];
        char ch0;
        int readfstate = 1;
        int playlen1 = 0;
        int playlen2 = 0;
        playkey1[playlen1] = '\0';
        playkey2[playlen2] = '\0';
        while ((ch0 = fgetc(input)) != EOF && ch0 != '\n' && ch0 != '\r')
        {
            if (readfstate == 1)
            {
                if (ch0 == ',')
                {
                    readfstate = 2;
                    continue;
                }
                else if (playlen1 < 21)
                {
                    playkey1[playlen1] = ch0;
                    playlen1++;
                    playkey1[playlen1] = '\0';
                    continue;
                }
                //it is reading the first vertex in the line map
            }
            if (readfstate == 2)
            {
                if (playlen2 < 21)
                {
                    playkey2[playlen2] = ch0;
                    playlen2++;
                    playkey2[playlen2] = '\0';
                    continue;
                }
                //it is reading the second vertex in the line map
            }
        }
        if (ch0 == '\r')
        {
            ch0 = fgetc(input);
        }//sneak into the next line
        if (playlen1 == 0 || playlen2 == 0)
        {
            break;
            //invalid input or EOF has met
        }
        undigraph_add_edge(g, playkey1, playkey2);
    }
    if (g->size == 0)
    {
        undigraph_destroy(g);
        fclose(input);
        return 0;
        //the graph is empty
    }
    FILE* in = stdin;
    if (!in)
    {
        undigraph_destroy(g);
        fclose(input);
        return 0;
    }
    while (true)
    {
        char querykey1[21];
        char querykey2[21];
        int querylen1 = 0;
        int querylen2 = 0;
        int readstate = 1;
        querykey1[querylen1] = '\0';
        querykey2[querylen2] = '\0';
        char ch;
        while ((ch = fgetc(in)) != EOF && ch != '\n' && ch != '\r')
        {
            if (readstate == 1)
            {
                if (ch == ',')
                {
                    // start reading distribution of units
                    readstate = 2;
                    continue;
                }
                else if (querylen1 < 21)
                {
                    querykey1[querylen1] = ch;
                    querylen1++;
                    querykey1[querylen1] = '\0';
                    continue;
                }
            }
            if (readstate == 2)
            {
                if (querylen2 < 21)
                {
                    querykey2[querylen2] = ch;
                    querylen2++;
                    querykey2[querylen2] = '\0';
                    continue;
                }
            }
        }
        if (ch == '\r')
        {
            ch = fgetc(in);
        }
        //read in the query names by the same method
        if (querylen1 == 0 || querylen2 == 0)
        {
            break;
        }
        if (!undigraph_has_vertex(g, querykey1) || !undigraph_has_vertex(g, querykey2))
        {
            break;
        }
        //queries were read in and in the graph
        if (strcmp(argv[2], "distance") == 0)
        {
            if (strcmp(querykey1, querykey2) == 0)
            {
                printf("%s and %s are the same person\n", querykey1, querykey2);
                continue;
            }
            size_t dist = undigraph_bfs_distance(g, querykey1, querykey2);
            if (dist > 0)
            {
                printf("%s, %s have minimum path length %d\n", querykey1, querykey2, dist);
                continue;
            }
            if (dist == 0)
            {
                printf("%s, %s are not connected\n", querykey1, querykey2);
                continue;
            }
        }
        else if (strcmp(argv[2], "mutual") == 0)
        {
            size_t found = 0;
            char** mutfr0 = undigraph_common_neighbors(g, querykey1, querykey2, &found, false);
            char** mutfr = malloc(sizeof(char*) * found);
            //create a name tag list with the length as the mutual length since the length of undigraph_common_neighbors may not be
            if (found == 0)
            {
                printf("%s, %s have no mutual friends\n", querykey1, querykey2);
                free(mutfr);
                free(mutfr0);
                continue;
            }//nothing found
            for (int p = 0; p < found; p++)
            {
                mutfr[p] = mutfr0[p];
            }
            qsort(mutfr, found, sizeof(char*), string_ptr_compare);
            printf("%s, %s have mutual friends", querykey1, querykey2);
            for (int p = 0; p < found; p++)
            {
                printf(" %s", mutfr[p]);
            }
            printf("\n");
            for (int p = 0; p < found; p++)
            {
                free(mutfr[p]);

            }
            free(mutfr);
            free(mutfr0);
            continue;
        }
        else if (strcmp(argv[2], "both") == 0)
        {
            if (strcmp(querykey1, querykey2) == 0)
            {
                printf("%s and %s are the same person\n", querykey1, querykey2);
            }
            if (strcmp(querykey1, querykey2) != 0)
            {
                size_t dist = undigraph_bfs_distance(g, querykey1, querykey2);
                if (dist > 0)
                {
                    printf("%s, %s have minimum path length %d\n", querykey1, querykey2, dist);
                }
                if (dist == 0)
                {
                    printf("%s, %s are not connected\n", querykey1, querykey2);
                }
            }
            size_t found = 0;
            char** mutfr0 = undigraph_common_neighbors(g, querykey1, querykey2, &found, false);
            char** mutfr = malloc(sizeof(char*) * found);
            if (found == 0)
            {
                printf("%s, %s have no mutual friends\n", querykey1, querykey2);
                free(mutfr);
                free(mutfr0);
                continue;
            }
            for (int p = 0; p < found; p++)
            {
                mutfr[p] = mutfr0[p];
            }
            qsort(mutfr, found, sizeof(char*), string_ptr_compare);
            printf("%s, %s have mutual friends", querykey1, querykey2);
            for (int p = 0; p < found; p++)
            {
                printf(" %s", mutfr[p]);
            }
            printf("\n");
            for (int p = 0; p < found; p++)
            {
                free(mutfr[p]);
            }
            free(mutfr);
            free(mutfr0);
            continue;
            //a combination of mutual and distance
        }
    }
    undigraph_destroy(g);
    fclose(input);
    fclose(in);
    return 0;
}