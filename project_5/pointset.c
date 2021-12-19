#include <stdbool.h>
#include <stddef.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "point2d.h"
#include "pointset.h"

typedef struct _point_node pointnd;

struct _point_node
{
    pointnd* NW;
    pointnd* NE;
    pointnd* SW;
    pointnd* SE;
    point2d* the_point;
};
//The node in the tree

typedef struct _point_region
{
    double priority;
    point2d p1;
    point2d p2;
    pointnd* p3;
} region;
//The region containing lowerleft corner, the point of the region and the upperright corner point

struct _pointset
{
    pointnd* root;
    size_t size;
};

void pointset_for_each_helper(pointnd* curr, void (*f)(const point2d*, void*), void* arg);//Help going through each node in the tree
int compare_x_coordinates(const void* p1, const void* p2);//Help qsort to sort by x coordinates
int relative_direction(point2d* p1, point2d* p2);//Determine the relative direction of 2 points
void pointset_create_helper(pointnd* need, size_t l, point2d* pts);//Help creating a function
void swap_the_point(region* target, int u1, int u2);//Swap 2 regions in an array with position u1 and u2
void enqueue(region* target, double r1, double r2, double r3, double r4, double r5, pointnd* pnd, int rr);//put new elements in the heap of regions
region* dequeue(region* target, int rr);//Take the region with lowest priority in the heap
void pointset_nearest_neighbor_helper(pointnd* t, point2d* query, point2d* neighbor, double* d, point2d* lowerleft, point2d* upperright);//Help determining the nearest neighbor

pointset* pointset_create(const point2d* pts, size_t n)
{
    pointset* result = malloc(sizeof(pointset));
    result->root = NULL;
    result->size = 0;
    if (pts == NULL || n == 0)
    {
        return result;
    }
    //The case that array is NULL
    point2d* newpts = malloc(sizeof(point2d) * n);
    for (int k = 0; k < n; k++)
    {
        newpts[k].x = pts[k].x;
        newpts[k].y = pts[k].y;
    }
    qsort(newpts, n, sizeof(point2d), compare_x_coordinates);
    //Sort the point2d array by x coordinates
    result->root = malloc(sizeof(pointnd));
    result->root->the_point = malloc(sizeof(point2d));
    pointset_create_helper(result->root, n, newpts);
    //build the tree by recursion
    result->size = n;
    free(newpts);
    return result;
}

size_t pointset_size(const pointset* t)
{
    //Take the size of the tree
    return t->size;
}

bool pointset_add(pointset* t, const point2d* pt)
{
    if (pt == NULL)
    {
        return false;
    }
    pointnd* curr = t->root;
    while (true)
    {
        if (t->root == NULL)
        {
            t->root = malloc(sizeof(pointnd));
            t->root->the_point = malloc(sizeof(point2d));
            t->root->NE = NULL;
            t->root->SW = NULL;
            t->root->NW = NULL;
            t->root->SE = NULL;
            t->root->the_point->x = pt->x;
            t->root->the_point->y = pt->y;
            t->size++;
            return true;
            //If root is NULL, add the point to root
        }
        if (curr->the_point->x == pt->x && curr->the_point->y == pt->y)
        {
            //If the incoming point exists, return false
            return false;
        }
        else
        {
            if (pt->x <= curr->the_point->x && pt->y >= curr->the_point->y)
            {
                if (curr->NW == NULL)
                {
                    curr->NW = malloc(sizeof(pointnd));
                    curr->NW->the_point = malloc(sizeof(point2d));
                    curr->NW->NE = NULL;
                    curr->NW->SW = NULL;
                    curr->NW->NW = NULL;
                    curr->NW->SE = NULL;
                    curr->NW->the_point->x = pt->x;
                    curr->NW->the_point->y = pt->y;
                    t->size++;
                    return true;
                }
                curr = curr->NW;
                continue;
            }
            if (pt->x <= curr->the_point->x && pt->y <= curr->the_point->y)
            {
                if (curr->SW == NULL)
                {
                    curr->SW = malloc(sizeof(pointnd));
                    curr->SW->the_point = malloc(sizeof(point2d));
                    curr->SW->NE = NULL;
                    curr->SW->SW = NULL;
                    curr->SW->NW = NULL;
                    curr->SW->SE = NULL;
                    curr->SW->the_point->x = pt->x;
                    curr->SW->the_point->y = pt->y;
                    t->size++;
                    return true;
                }
                curr = curr->SW;
                continue;
            }
            if (pt->x >= curr->the_point->x && pt->y >= curr->the_point->y)
            {
                if (curr->NE == NULL)
                {
                    curr->NE = malloc(sizeof(pointnd));
                    curr->NE->the_point = malloc(sizeof(point2d));
                    curr->NE->NE = NULL;
                    curr->NE->SW = NULL;
                    curr->NE->NW = NULL;
                    curr->NE->SE = NULL;
                    curr->NE->the_point->x = pt->x;
                    curr->NE->the_point->y = pt->y;
                    t->size++;
                    return true;
                }
                curr = curr->NE;
                continue;
            }
            if (pt->x >= curr->the_point->x && pt->y <= curr->the_point->y)
            {
                if (curr->SE == NULL)
                {
                    curr->SE = malloc(sizeof(pointnd));
                    curr->SE->the_point = malloc(sizeof(point2d));
                    curr->SE->NE = NULL;
                    curr->SE->SW = NULL;
                    curr->SE->NW = NULL;
                    curr->SE->SE = NULL;
                    curr->SE->the_point->x = pt->x;
                    curr->SE->the_point->y = pt->y;
                    t->size++;
                    return true;
                }
                curr = curr->SE;
                continue;
            }
            //Keep looking for the position of the incoming point by searching down the tree and add it
        }
    }
}

bool pointset_contains(const pointset* t, const point2d* pt)
{
    if (t->root == NULL)
    {
        return false;
    }
    pointnd* curr = t->root;
    point2d temp;
    temp.x = pt->x;
    temp.y = pt->y;
    while (true)
    {
        if (curr == NULL)
        {
            //cannot find the point down its position in the tree
            return false;
        }
        if (curr->the_point->x == pt->x && curr->the_point->y == pt->y)
        {
            return true;
        }
        else
        {
            int direction = relative_direction(&temp, curr->the_point);
            if (direction == 1)
            {
                curr = curr->NW;
                continue;
            }
            if (direction == 2)
            {
                curr = curr->SW;
                continue;
            }
            if (direction == 3)
            {
                curr = curr->NE;
                continue;
            }
            if (direction == 4)
            {
                curr = curr->SE;
                continue;
            }
            //search for the position of the given point down the tree
        }
    }
}
void pointset_nearest_neighbor(const pointset* t, const point2d* pt, point2d* neighbor, double* d)
{
    if (t == NULL || t->root == NULL || pt == NULL)
    {
        *d = INFINITY;
        return;
    }
    point2d* startlowerleft = malloc(sizeof(point2d));
    point2d* startupperright = malloc(sizeof(point2d));
    startlowerleft->x = -INFINITY;
    startlowerleft->y = -INFINITY;
    startupperright->x = INFINITY;
    startupperright->y = INFINITY;
    *d = INFINITY;
    neighbor->x = t->root->the_point->x;
    neighbor->y = t->root->the_point->y;
    point2d temp;
    temp.x = pt->x;
    temp.y = pt->y;
    //Start with the root and its region (who space) before recursion
    pointset_nearest_neighbor_helper(t->root, &temp, neighbor, d, startlowerleft, startupperright);
    free(startlowerleft);
    free(startupperright);
}

point2d* pointset_k_nearest(const pointset* t, const point2d* pt, size_t k)
{
    size_t n = t->size;
    if (t->root == NULL)
    {
        return NULL;
    }
    region* queue = malloc(sizeof(region) * n);
    point2d* nearest = malloc(sizeof(point2d) * k);
    double dist;
    pointnd* curr0 = t->root;
    int numinqueue = 0;
    int found = 0;
    double downleftx = -INFINITY;
    double downlefty = -INFINITY;
    double uprightx = INFINITY;
    double uprighty = INFINITY;
    point2d* urcorner = malloc(sizeof(point2d));
    point2d* dlcorner = malloc(sizeof(point2d));
    //Start with the root and its region (who space) before recursion
    enqueue(queue, uprightx, uprighty, downleftx, downlefty, 0, t->root, numinqueue);
    numinqueue++;
    int index = 0;
    while (numinqueue != 0 && found < k)
    {
        index++;
        region* curr = dequeue(queue, numinqueue);
        //Dequeue the first element in the heap
        curr0 = curr->p3;
        double r1 = curr->p1.x;
        double r2 = curr->p1.y;
        double r3 = curr->p2.x;
        double r4 = curr->p2.y;
        numinqueue--;
        if (r1 != 3724.383)//If the element is a point instead of region, the lowerleft and upperright corner are set to be 3724.383
        {
            //The result of dequeue is a region
            double ptdist = point2d_distance(curr0->the_point, pt);
            if (curr0->NE != NULL)
            {
                uprightx = r1;
                uprighty = r2;
                downleftx = curr0->the_point->x;
                downlefty = curr0->the_point->y;
                urcorner->x = uprightx;
                urcorner->y = uprighty;
                dlcorner->x = downleftx;
                dlcorner->y = downlefty;
                dist = point2d_distance_to_rectangle(pt, dlcorner, urcorner);
                enqueue(queue, uprightx, uprighty, downleftx, downlefty, dist, curr0->NE, numinqueue);
                numinqueue++;
            }
            if (curr0->SW != NULL)
            {
                uprightx = curr0->the_point->x;
                uprighty = curr0->the_point->y;
                downleftx = r3;
                downlefty = r4;
                urcorner->x = uprightx;
                urcorner->y = uprighty;
                dlcorner->x = downleftx;
                dlcorner->y = downlefty;
                dist = point2d_distance_to_rectangle(pt, dlcorner, urcorner);
                enqueue(queue, uprightx, uprighty, downleftx, downlefty, dist, curr0->SW, numinqueue);
                numinqueue++;

            }
            if (curr0->SE != NULL)
            {
                uprightx = r1;
                uprighty = curr0->the_point->y;
                downleftx = curr0->the_point->x;
                downlefty = r4;
                urcorner->x = uprightx;
                urcorner->y = uprighty;
                dlcorner->x = downleftx;
                dlcorner->y = downlefty;
                dist = point2d_distance_to_rectangle(pt, dlcorner, urcorner);
                enqueue(queue, uprightx, uprighty, downleftx, downlefty, dist, curr0->SE, numinqueue);
                numinqueue++;
            }
            if (curr0->NW != NULL)
            {
                uprightx = curr0->the_point->x;
                uprighty = r2;
                downleftx = r3;
                downlefty = curr0->the_point->y;
                urcorner->x = uprightx;
                urcorner->y = uprighty;
                dlcorner->x = downleftx;
                dlcorner->y = downlefty;
                dist = point2d_distance_to_rectangle(pt, dlcorner, urcorner);
                enqueue(queue, uprightx, uprighty, downleftx, downlefty, dist, curr0->NW, numinqueue);
                numinqueue++;
            }
            //If the children are not NULL, enqueue their corresponding areas
            enqueue(queue, 3724.383, 3724.383, 3724.383, 3724.383, ptdist, curr0, numinqueue);
            //enqueue the point of the region,  the lowerleft and upperright corner are set to be 3724.383
            numinqueue++;
        }
        else
        {
            //If the element dequeued is a point but not a region
            nearest[found].x = curr0->the_point->x;
            nearest[found].y = curr0->the_point->y;
            found++;
        }
    }
    free(urcorner);
    free(dlcorner);
    free(queue);
    return nearest;
}

void pointset_for_each(const pointset* t, void (*f)(const point2d*, void*), void* arg)
{
    if (t == NULL || f == NULL)
    {
        return;
    }
    pointset_for_each_helper(t->root, f, arg);
}

void pointset_destroy(pointset* t)
{
    if (t == NULL)
    {
        return;
    }
    if (t->root == NULL)
    {
        free(t);
        return;
    }
    size_t n = t->size;
    pointnd** allnodes = malloc(sizeof(pointnd*) * n);
    int layercheckA = 0;
    int layercheckB = 1;
    int recent = 1;
    allnodes[0] = t->root;
    while (recent < n)
    {
        for (int i = layercheckA; i < layercheckB; i++)
        {
            if (allnodes[i]->NE != NULL)
            {
                allnodes[recent] = allnodes[i]->NE;
                recent++;
            }
            if (allnodes[i]->NW != NULL)
            {
                allnodes[recent] = allnodes[i]->NW;
                recent++;
            }
            if (allnodes[i]->SE != NULL)
            {
                allnodes[recent] = allnodes[i]->SE;
                recent++;
            }
            if (allnodes[i]->SW != NULL)
            {
                allnodes[recent] = allnodes[i]->SW;
                recent++;
            }
        }
        layercheckA = layercheckB;
        layercheckB = recent;
    }
    //build an array of pointers pointing to all the elements in the tree
    for (int j = 0; j < n; j++)
    {
        free(allnodes[j]->the_point);
        free(allnodes[j]);
    }
    //free them through the array of pointers one by one
    //cannot use recursion to avoid stack overflow
    free(allnodes);
    free(t);
}

//=================================================HELPER FUNCTIONS=======================================================

void pointset_for_each_helper(pointnd* curr, void (*f)(const point2d*, void*), void* arg)
{
    if (curr != NULL)
    {
        pointset_for_each_helper(curr->NW, f, arg);
        pointset_for_each_helper(curr->NE, f, arg);
        f(curr->the_point, arg);
        pointset_for_each_helper(curr->SW, f, arg);
        pointset_for_each_helper(curr->SE, f, arg);
    }
    //recursion through the tree
}

int compare_x_coordinates(const void* p1, const void* p2)
{
    if (((point2d*)p1)->x >= ((point2d*)p2)->x)
    {
        return 1;
    }
    if (((point2d*)p1)->x < ((point2d*)p2)->x)
    {
        return -1;
    }
}

int relative_direction(point2d* p1, point2d* p2)
{
    if (p1->x <= p2->x && p1->y >= p2->y)
    {
        return 1;
    }
    //p1 at the NW of p2
    if (p1->x <= p2->x && p1->y <= p2->y)
    {
        return 2;
    }
    //p1 at the SW of p2
    if (p1->x >= p2->x && p1->y >= p2->y)
    {
        return 3;
    }
    //p1 at the NE of p2
    if (p1->x >= p2->x && p1->y <= p2->y)
    {
        return 4;
    }
    //p1 at the SE of p2
}

void pointset_create_helper(pointnd* need, size_t l, point2d* pts)
{
    point2d* ptsinNW = malloc(sizeof(point2d) * l);
    point2d* ptsinNE = malloc(sizeof(point2d) * l);
    point2d* ptsinSW = malloc(sizeof(point2d) * l);
    point2d* ptsinSE = malloc(sizeof(point2d) * l);
    size_t numofNW = 0;
    size_t numofNE = 0;
    size_t numofSW = 0;
    size_t numofSE = 0;
    int mid = l / 2;
    need->the_point->x = pts[mid].x;
    need->the_point->y = pts[mid].y;
    need->NW = NULL;
    need->NE = NULL;
    need->SW = NULL;
    need->SE = NULL;
    for (int i = 0; i < l; i++)
    {
        if (i == mid)
        {
            continue;
        }
        int direction = relative_direction(&pts[i], &pts[mid]);
        if (direction == 1)
        {
            ptsinNW[numofNW] = pts[i];
            numofNW++;
            continue;
        }
        if (direction == 2)
        {
            ptsinSW[numofSW] = pts[i];
            numofSW++;
            continue;
        }//SW
        if (direction == 3)
        {
            ptsinNE[numofNE] = pts[i];
            numofNE++;
            continue;
        }//NE
        if (direction == 4)
        {
            ptsinSE[numofSE] = pts[i];
            numofSE++;
            continue;
        }//SE
    }
    //create 4 arrays with each containg points in 4 areas of a given point and start recusion to build each subtree
    if (numofNW != 0)
    {
        need->NW = malloc(sizeof(pointnd));
        need->NW->the_point = malloc(sizeof(point2d));
        pointset_create_helper(need->NW, numofNW, ptsinNW);
    }
    if (numofNE != 0)
    {
        need->NE = malloc(sizeof(pointnd));
        need->NE->the_point = malloc(sizeof(point2d));
        pointset_create_helper(need->NE, numofNE, ptsinNE);
    }
    if (numofSW != 0)
    {
        need->SW = malloc(sizeof(pointnd));
        need->SW->the_point = malloc(sizeof(point2d));
        pointset_create_helper(need->SW, numofSW, ptsinSW);
    }
    if (numofSE != 0)
    {
        need->SE = malloc(sizeof(pointnd));
        need->SE->the_point = malloc(sizeof(point2d));
        pointset_create_helper(need->SE, numofSE, ptsinSE);
    }
    free(ptsinNW);
    free(ptsinNE);
    free(ptsinSW);
    free(ptsinSE);
}

void pointset_nearest_neighbor_helper(pointnd* t, point2d* query, point2d* neighbor, double* d, point2d* lowerleft, point2d* upperright)
{
    if (t == NULL)
    {
        return;
    }
    double rectandist = point2d_distance_to_rectangle(query, lowerleft, upperright);
    double pointsdist = point2d_distance(query, t->the_point);
    if (rectandist >= *d) // No need to continue if the given point is even farther than the rectangle region
    {
        return;
    }
    if (pointsdist <= *d) // Find nearer neighbors and update it
    {
        *d = pointsdist;
        neighbor->x = t->the_point->x;
        neighbor->y = t->the_point->y;
    }
    point2d* newlowerleft = malloc(sizeof(point2d));
    point2d* newupperright = malloc(sizeof(point2d));
    newupperright->x = upperright->x;
    newupperright->y = upperright->y;
    newlowerleft->x = t->the_point->x;
    newlowerleft->y = t->the_point->y;
    pointset_nearest_neighbor_helper(t->NE, query, neighbor, d, newlowerleft, newupperright);
    newlowerleft->x = lowerleft->x;
    newlowerleft->y = lowerleft->y;
    newupperright->x = t->the_point->x;
    newupperright->y = t->the_point->y;
    pointset_nearest_neighbor_helper(t->SW, query, neighbor, d, newlowerleft, newupperright);
    newupperright->x = t->the_point->x;
    newupperright->y = upperright->y;
    newlowerleft->x = lowerleft->x;
    newlowerleft->y = t->the_point->y;
    pointset_nearest_neighbor_helper(t->NW, query, neighbor, d, newlowerleft, newupperright);
    newupperright->x = upperright->x;
    newupperright->y = t->the_point->y;
    newlowerleft->x = t->the_point->x;
    newlowerleft->y = lowerleft->y;
    pointset_nearest_neighbor_helper(t->SE, query, neighbor, d, newlowerleft, newupperright);
    //recursion through all 4 children areas with updated rectangle
    free(newlowerleft);
    free(newupperright);
}

void swap_the_point(region* target, int u1, int u2)
{
    double x1 = target[u1].p1.x;
    double x2 = target[u1].p1.y;
    double x3 = target[u1].p2.x;
    double x4 = target[u1].p2.y;
    double x5 = target[u1].priority;
    pointnd* pnd = target[u1].p3;
    target[u1].p1.x = target[u2].p1.x;
    target[u1].p1.y = target[u2].p1.y;
    target[u1].p2.x = target[u2].p2.x;
    target[u1].p2.y = target[u2].p2.y;
    target[u1].p3 = target[u2].p3;
    target[u1].priority = target[u2].priority;
    target[u2].p1.x = x1;
    target[u2].p1.y = x2;
    target[u2].p2.x = x3;
    target[u2].p2.y = x4;
    target[u2].priority = x5;
    target[u2].p3 = pnd;
}

void enqueue(region* target, double r1, double r2, double r3, double r4, double r5, pointnd* pnd, int rr)
{
    target[rr].p1.x = r1;
    target[rr].p1.y = r2;
    target[rr].p2.x = r3;
    target[rr].p2.y = r4;
    target[rr].priority = r5;
    target[rr].p3 = pnd;
    if (rr == 0)
        return;
    int parent = (rr - 1) / 2;
    while (target[rr].priority < target[parent].priority)
    {
        swap_the_point(target, rr, parent);
        rr = parent;
        if (rr == 0)
            break;
        parent = (rr - 1) / 2;
    }
    //if parent's priority is larger, swap the given point and its parent until the root is met
}

region* dequeue(region* target, int rr)
{
    int start = 0;
    rr = rr - 1;
    if (rr == 0)
    {
        return &target[0];
    }
    if (rr == 1)
    {
        swap_the_point(target, 0, 1);
        return &target[1];
    }
    //If there are only 2 and 1 elements
    swap_the_point(target, rr, start);
    //swap the first and last points in the heap
    while (true)
    {
        if (2 * start + 1 >= rr)
        {
            break;
        }
        if (2 * start + 2 == rr)
        {
            if (target[start].priority > target[2 * start + 1].priority)
            {
                swap_the_point(target, 2 * start + 1, start);
                break;
            }
            else
            {
                break;
            }
        }
        if (target[start].priority > target[2 * start + 1].priority || target[start].priority > target[2 * start + 2].priority) // memory use!
        {
            if (target[2 * start + 2].priority > target[2 * start + 1].priority)
            {
                swap_the_point(target, 2 * start + 1, start);
                start = 2 * start + 1;
                continue;
            }
            if (target[2 * start + 2].priority <= target[2 * start + 1].priority)
            {
                swap_the_point(target, 2 * start + 2, start);
                start = 2 * start + 2;
                continue;
            }

        }
        break;
        //if the given node is with larger priority than its children, swap it with the children of smaller priority until the leaf
    }
    return &target[rr];
    //Return the last element in the heap which used to be the first one before swap
}
//The heap is with its smaller one on the top