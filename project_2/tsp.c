#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "location.h"
double total_distance(location* locs, int n); //calculate the whole distance of a whole journey
int* swap_ad(int n, location* locs); //find the final order of least journey length with adjacent swap
void swap_location(location* locs, int i, int j, int n);// swap the j th and i th location in the array
void swap_int(int* locs, int i, int j, int n); // swap the j th and i th integer in the array
int* swap_ra(int n, location* locs); //find the final order of least journey length with any swap
int* farthest(int n, location* dest); //find the final order of journey length with the farthest algorithm
void homogenize(location* locs, int* index, location value, int n, int a); //make all elements in the array locs that have the same index number in the index array as the same "value"
int is_even(int n);//To know if n is even or odd
int* reordering(int* arr, int n); // if the start of the city is not the initial start city, reorder it to be
double orderedistance(location* locs, int n, int* order); //know the journey distance with cities of certain orders

/*When names and locations are read in, they are automatically tagged with a number starting from 0, like if {HVN, FLL} were read in, they are tagged with number 0 and 1
All later operations will be based on the tagged number, and finally with the tagged number, we need to calculate the whole distance with
the tagged number as tranverlling order.
and reconstruct the journey order
For example, if we have read in HVN RSW PBI TPA FLL MCO, they will be assigned with 0, 1, 2, 3, 4, 5. If the final number order is 0, 2, 1, 3, 4, 5, this indicates that the tranvelling
order is HVN PBI RSW TPA FLL MCO */

int main(int argc, char* argv[])
{
    int n;
    location no_use;
    if (argc < 2)
    {
        fprintf(stderr, "TSP: missing filename\n");//No file names
        return 0;
    }
    FILE* input = fopen(argv[1], "r");
    if (!input)
    {
        fprintf(stderr, "TSP: could not open %s\n", argv[1]);//not valid file
        return 0;
    }
    fscanf(input, "%d", &n);
    if (n < 2)
    {
        fprintf(stderr, "TSP: too few cities\n");//cities less than 2
        return 0;
    }
    char city[n][4];
    location* cities = malloc(n * sizeof(no_use));
    for (int i = 0; i < n; i++)
    {
        fscanf(input, "%s", &city[i]);
        city[i][3] = '\0';
    }//assign read city names to the array of city names
    for (int i = 0; i < n; i++)
    {
        fscanf(input, "%lf %lf", &cities[i].lat, &cities[i].lon);
    }//assign read locations to the array of locations
    for (int y = 2; y < argc; y++)
    {
        if (strcmp(argv[y], "-given") == 0)
        {
            printf("-given            :");
            printf("%13.2lf ", total_distance(cities, n));//print the total distances tranvelling according to the order
            for (int b = 0; b < n; b++)
            {
                printf("%s ", city[b]);
            }//print the city names according to the order
            printf("%s", city[0]);
            printf("\n");
            continue;
        }
        if (strcmp(argv[y], "-farthest") == 0)
        {
            location* temp_loc = malloc(n * sizeof(*cities));
            for (int i = 0; i < n; i++)
            {
                temp_loc[i] = cities[i];
            }
            int* farthestorder = farthest(n, temp_loc);//Need a new location array to be passed into functions since functions used pass by reference
            free(temp_loc);
            printf("-farthest         :");
            printf("%13.2lf ", orderedistance(cities, n, farthestorder));
            for (int b = 0; b < n; b++)
            {
                printf("%s ", city[farthestorder[b]]);
            }
            printf("%s", city[0]);
            printf("\n");
            free(farthestorder);
            continue;
        }
        if (strcmp(argv[y], "-exchange") == 0)
        {
            if (y == argc - 1)
            {
                fprintf(stderr, "TSP: invalid algorithm arguments\n");
                free(cities);
                return 0;
            }
            if (strcmp(argv[y + 1], "adjacent") == 0)
            {
                location* temp_loc = malloc(n * sizeof(*cities));
                for (int i = 0; i < n; i++)
                {
                    temp_loc[i] = cities[i];
                }
                int* swap_adjacent = swap_ad(n, temp_loc);
                free(temp_loc);
                printf("-exchange adjacent:");
                printf("%13.2lf ", orderedistance(cities, n, swap_adjacent));
                for (int b = 0; b < n; b++)
                {
                    printf("%s ", city[swap_adjacent[b]]);
                }
                printf("%s", city[0]);
                printf("\n");
                y++;
                free(swap_adjacent);
                continue;
            }
            if (strcmp(argv[y + 1], "any") == 0)
            {
                location* temp_loc = malloc(n * sizeof(*cities));
                for (int i = 0; i < n; i++)
                {
                    temp_loc[i] = cities[i];
                }
                int* swap_any = swap_ra(n, temp_loc);
                free(temp_loc);
                printf("-exchange any     :");
                printf("%13.2lf ", orderedistance(cities, n, swap_any));
                for (int b = 0; b < n; b++)
                {
                    printf("%s ", city[swap_any[b]]);
                }
                printf("%s", city[0]);
                printf("\n");
                y++;
                free(swap_any);
                continue;
            }
            else
            {
                fprintf(stderr, "TSP: invalid algorithm arguments\n");
                free(cities);
                return 0;
            }
        }
        else
        {
            fprintf(stderr, "TSP: invalid algorithm arguments\n");
            free(cities);
            return 0;
        }
    }
    free(cities);
    fclose(input);
    return 0;
}


int* swap_ra(int n; location* locs)
{
    double distance;
    int* order = malloc(n * sizeof(int));
    int loop = 1;
    for (int u = 0; u < n; u++)
    {
        order[u] = u;
    }
    while (loop == 1)
    {
        int index1 = 0;
        int index2 = 0;
        double dis_redu = 0;
        distance = total_distance(locs, n);
        for (int i = 0; i < n; i++)
        {
            for (int j = (i + 1); j < n; j++)
            {
                swap_location(locs, i, j, n);
                if (distance - total_distance(locs, n) > dis_redu)
                {
                    index1 = i; index2 = j;
                    dis_redu = distance - total_distance(locs, n);
                }
                swap_location(locs, i, j, n);
            }// interate through all n(n-1)/2 ways of swap
        }
        if (dis_redu == 0)
        {
            break;
        }
        swap_location(locs, index1, index2, n);
        swap_int(order, index1, index2, n);
    }
    int* real_order;
    real_order = reordering(order, n);
    free(order);
    return real_order;
}

int* farthest(int n, location* dest)
{
    int index = 0;//The one maximized the distance to the index1 ones
    int index1 = 0;//The one used to be compared with all other cities for distance
    int* order = malloc(n * sizeof(int));
    order[0] = 0;
    int* order1 = malloc(n * sizeof(int));
    for (int j = 0; j < n; j++)
    {
        order1[j] = index1;
        location temp = dest[index1];
        double distance = 0;
        for (int i = 0; i < n; i++)
        {
            if (distance < location_distance(&dest[i], &temp))
            {
                distance = location_distance(&dest[i], &temp);
                index = i;
            }
        }
        homogenize(dest, order1, dest[index], n, j);//set all assigned locations the same to avoid repeated comparisons
        index1 = index;
        if (is_even(j) == 1)
        {
            order[n - j / 2 - 1] = index;//even number assigned at the end of array
            continue;
        }
        else
        {
            order[(j + 1) / 2] = index;//odd number assigned at the start of array
            continue;
        }
        index = 0;
    }
    free(order1);
    return order;
}


//====================================================================HELP FUNCTIONS================================================================

void swap_location(location* locs, int i, int j, int n)
{
    location temp = locs[i];
    locs[i] = locs[j];
    locs[j] = temp;
}

void swap_int(int* locs, int i, int j, int n)
{
    if (i == n - 1)
        j = 0;
    int temp = locs[i];
    locs[i] = locs[j];
    locs[j] = temp;
}

int is_even(int n)
{
    if (n % 2 == 0)
        return 1;
    else
        return 0;
}

void homogenize(location* locs, int* index, location value, int n, int a)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < a + 1; j++)
        {
            if (i == index[j])
            {
                locs[i] = value;
                break;
            }
        }
    }
}

int* reordering(int* arr, int n)
{
    int start;
    int* reordered = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        if (arr[i] == 0)
        {
            start = i;
            break;
        }
    }
    for (int i = 0; i < n; i++)
    {
        if ((i + start) <= (n - 1))
        {
            reordered[i] = arr[i + start];
            continue;
        }
        else
        {
            reordered[i] = arr[i + start - n];
            continue;
        }
    }//reorder in the positive direction
    if (reordered[1] > reordered[n - 1])//if the second city in the computed tour appears in the input before the penultimate city
    {
        for (int i = 0; i < n; i++)
        {
            if ((start - i) >= 0)
            {
                reordered[i] = arr[start - i];
                continue;
            }
            else
            {
                reordered[i] = arr[n + start - i];
                continue;
            }
        }//reorder in a reverse way
    }
    return reordered;
}

double total_distance(location* locs, int n)
{
    double distance = 0;
    for (int i = 0; i < (n - 1); i++)
    {
        distance = distance + location_distance(&locs[i], &locs[i + 1]);
    }
    distance = distance + location_distance(&locs[0], &locs[n - 1]);
    return distance;
}

double orderedistance(location* locs, int n, int* order)
{
    double distance = 0;
    for (int i = 0; i < (n - 1); i++)
    {
        distance = distance + location_distance(&locs[order[i]], &locs[order[i + 1]]);
    }
    distance = distance + location_distance(&locs[order[0]], &locs[order[n - 1]]);
    return distance;
}