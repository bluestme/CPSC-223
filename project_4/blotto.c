#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "entry.h"
#include"gmap.h"
#include"string_key.h"
int comparescore(const void* p1, const void* p2);
int comparewinning(const void* p1, const void* p2);
void update_score(const void* key1, const void* key2, gmap* m, int total_battle, int* battlevalue);
gmap_node** all_gmap_nodes(gmap* m);

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        return 0;
    }
    if (!fopen(argv[1], "r"))
    {
        return 0;
    }
    if (strcmp(argv[2], "win") != 0 && strcmp(argv[2], "score") != 0)
    {
        return 0;
    }
    // invalid input cases
    int battlenum = argc - 3;
    int* battledist = malloc(sizeof(int) * battlenum);
    for (int k = 0; k < battlenum; k++)
    {
        battledist[k] = atoi(argv[3 + k]);
    }
    //input the distribution of scores on battle fields
    gmap* plays = gmap_create(duplicate, compare_keys, hash29, free);
    //create the distribution of points in battlefields
    while (true)
    {
        entry curren = entry_read(stdin, 31, battlenum);
        if (curren.distribution == NULL || curren.id == NULL)
        {
            if (curren.distribution == NULL && curren.id == NULL)
            {
                // something wrong with the standard input
                gmap_node** I_have_to_do_it = all_gmap_nodes(plays);
                for (int m = 0; m < gmap_size(plays); m++)
                {
                    free(I_have_to_do_it[m]->node_dist);
                }
                gmap_destroy(plays);
                free(I_have_to_do_it);
                free(battledist);
                return 0;
            }
            free(curren.id);
            break;
        }
        gmap_put(plays, curren.id, curren.distribution);
        //Put the newly read in nodes into the map
        free(curren.id);
    }
    //================================Read in All Entries===========================================
    char playkey1[31];
    char playkey2[31];
    FILE* input = fopen(argv[1], "r");
    while (true)
    {
        if (fscanf(input, "%s %s", playkey1, playkey2) != EOF)
        {
            update_score(playkey1, playkey2, plays, battlenum, battledist);
            continue;
        }
        break;
    }
    gmap_node** unsorted = all_gmap_nodes(plays);
    //================================Update All Entries============================================
    int tablesize = gmap_size(plays);
    if (strcmp("win", argv[2]) == 0)
    {
        qsort(unsorted, tablesize, sizeof(gmap_node*), comparewinning);
        for (int m = 0; m < tablesize; m++)
        {
            if (unsorted[m]->values->matches != 0)
            {
                printf("  %.3lf %s\n", unsorted[m]->values->winning / unsorted[m]->values->matches, unsorted[m]->node_id);
            }
            free(unsorted[m]->node_dist);
            // ==================== print all average winning cases ======================
        }
        fclose(input);
        gmap_destroy(plays);
        free(unsorted);
        free(battledist);
        return 0;
    }
    if (strcmp("score", argv[2]) == 0)
    {
        qsort(unsorted, tablesize, sizeof(gmap_node*), comparescore);
        for (int m = 0; m < tablesize; m++)
        {
            if (unsorted[m]->values->matches != 0)
            {
                double printscore = unsorted[m]->values->score / unsorted[m]->values->matches;
                if (printscore >= 100)
                {
                    printf("%.3lf %s\n", unsorted[m]->values->score / unsorted[m]->values->matches, unsorted[m]->node_id);
                }
                else if (printscore >= 10)
                {
                    printf(" %.3lf %s\n", unsorted[m]->values->score / unsorted[m]->values->matches, unsorted[m]->node_id);
                }
                else
                {
                    printf("  %.3lf %s\n", unsorted[m]->values->score / unsorted[m]->values->matches, unsorted[m]->node_id);
                }
                // ==================== print all average scores with different lengths ======================
            }
            free(unsorted[m]->node_dist);
        }
        fclose(input);
        gmap_destroy(plays);
        free(unsorted);
        free(battledist);
        return 0;
    }
    //================================Sort All Entries============================================
}



int comparescore(const void* p1, const void* p2)// Tend to put those with larger average scores and lower entry id values to the front
{
    gmap_node* a = *(gmap_node**)p1;
    gmap_node* b = *(gmap_node**)p2;
    if (a->values->matches == 0 && b->values->matches == 0) return 0;
    if (a->values->matches == 0 && b->values->matches != 0) return 1;
    if (a->values->matches != 0 && b->values->matches == 0) return -1;
    double avgscoreA = a->values->score / a->values->matches;
    double avgscoreB = b->values->score / b->values->matches;
    if (avgscoreA < avgscoreB) return 1;
    if (avgscoreA == avgscoreB) return strcmp(a->node_id, b->node_id);
    if (avgscoreA > avgscoreB) return -1;
}

int comparewinning(const void* p1, const void* p2) // Tend to put those with larger average winning and lower entry id values to the front
{
    const gmap_node* a = *(const gmap_node**)p1;
    const gmap_node* b = *(const gmap_node**)p2;
    if (a->values->matches == 0 && b->values->matches == 0) return 0;
    if (a->values->matches == 0 && b->values->matches != 0) return 1;
    if (a->values->matches != 0 && b->values->matches == 0) return -1;
    double avgwinningA = a->values->winning / a->values->matches;
    double avgwinningB = b->values->winning / b->values->matches;
    if (avgwinningA < avgwinningB) return 1;
    if (avgwinningA == avgwinningB) return strcmp(a->node_id, b->node_id);
    if (avgwinningA > avgwinningB) return -1;
}