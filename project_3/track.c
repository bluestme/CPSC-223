#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "track.h"
#include "location.h"
#include "more_math.h"
#include "trackpoint.h"

typedef struct _trackpoint_wrap wrapoint;
struct _trackpoint_wrap
{
    trackpoint* trackp;
    struct _trackpoint_wrap* next;
    struct _trackpoint_wrap* prev;
};
//A struct that wraps a trackpoint, a pointer prev pointing to the previous one and a pointer next pointing to the next wrapoint
bool same_point(trackpoint* a, trackpoint* b);
// To know if they are the same point
double linear(double a1, double a2, double total, double present);
// calculate the linear interpolation
bool same_point(trackpoint* a, trackpoint* b)
{
    if (trackpoint_get_latitude(a) == trackpoint_get_latitude(b) && trackpoint_get_longitude(a) == trackpoint_get_longitude(b))
    {
        return true;
    }
    else
        return false;
}

double linear(double a1, double a2, double total, double present)
{
    return (a2 - a1) * (present / total) + a1;
}
struct _track
{
    wrapoint head;
    wrapoint tail;
    int size;
};
//a track with a wrapoints at each end and its size


track* track_create()
{
    track* l = malloc(sizeof(track));
    location* loc1 = location_create(0, 0);
    location* loc2 = location_create(0, 0);
    l->head.trackp = trackpoint_create(loc1, 0);
    l->tail.trackp = trackpoint_create(loc2, 0);
    //All properties within the trackpoints tail and head are set to be 0
    l->head.prev = NULL;
    l->tail.next = NULL;
    l->head.next = &l->tail;
    l->tail.prev = &l->head;
    l->size = 0;
    return l;
}

void track_destroy(track* l)
{
    if (l != NULL)
    {
        wrapoint* curr = l->head.next;
        while (curr->next != NULL)
        {
            curr = curr->next;
            trackpoint_destroy(curr->prev->trackp);
            free(curr->prev);
        }
        //destroy all wrapoints in the linked list
        trackpoint_destroy(l->head.trackp);
        trackpoint_destroy(l->tail.trackp);
        free(l);
        //destroy head and tail and free it
    }
}

size_t track_size(const track* t)
{
    size_t i = t->size;
    return i;
}

bool track_add_point(track* t, trackpoint* p)
{
    wrapoint* newtp = malloc(sizeof(wrapoint));
    if (newtp != NULL)
    {
        newtp->trackp = p;
        if ((trackpoint_get_time(p) > trackpoint_get_time(t->tail.prev->trackp)) || t->tail.prev == &t->head)
        {
            newtp->prev = t->tail.prev;
            newtp->next = &t->tail;
            t->tail.prev->next = newtp;
            t->tail.prev = newtp;
            t->size++;
            // if the time in p is bigger than all other trackpoints in the track, add it to the end of the track
            return true;
        }
        else
        {
            // if not, just return false
            free(newtp);
            return false;
        }
    }
}

double track_length(const track* t)
{
    double time_start;
    double time_end;
    time_start = trackpoint_get_time(t->head.next->trackp);
    time_end = trackpoint_get_time(t->tail.prev->trackp);
    if (time_start != 0 && time_end != 0)
    {
        return time_end - time_start;
    }
    else
        return 0;
}

location* track_get(const track* t, double time)
{
    wrapoint* curr = t->head.next;
    if (curr != NULL)
    {
        if (t->head.next == &t->tail || time<trackpoint_get_time(t->head.next->trackp) || time>trackpoint_get_time(t->tail.prev->trackp))
        {
            // if the track is empty, given time is smaller than the smallest, or the given time is larger than the largest time
            return NULL;
        }
        while (curr != &t->tail)
        {
            if ((trackpoint_get_time(curr->trackp) > time) && (trackpoint_get_time(curr->prev->trackp) != 0) && (trackpoint_get_time(curr->prev->trackp) < time))
            {
                double total_time = (trackpoint_get_time(curr->trackp)) - (trackpoint_get_time(curr->prev->trackp));
                double present_time = time - (trackpoint_get_time(curr->prev->trackp));
                double lat = linear(trackpoint_get_latitude(curr->prev->trackp), trackpoint_get_latitude(curr->trackp), total_time, present_time);
                double lon = linear(trackpoint_get_longitude(curr->prev->trackp), trackpoint_get_longitude(curr->trackp), total_time, present_time);
                return location_create(lat, lon);
                // If the given time is between 2 trackpoints
            }
            if (trackpoint_get_time(curr->trackp) == time)
            {
                double newlat = trackpoint_get_latitude(curr->trackp);
                double newlon = trackpoint_get_longitude(curr->trackp);
                return location_create(newlat, newlon);
                //If the time is equal to the time of one of the trackpoint
            }
            curr = curr->next;
        }
    }
    return NULL;
}

void track_for_each(const track* t, void (*f)(const trackpoint*, void*), void* arg)
{
    wrapoint* curr = t->head.next;
    while (curr != &t->tail)
    {
        f(curr->trackp, arg);
        curr = curr->next;
    }
}

void track_merge(track* dest, track* src)
{
    int l = 0;
    while (l < 1)
    {
        wrapoint* currA;
        wrapoint* currB;
        int k = 0;
        if (trackpoint_get_time(dest->head.next->trackp) > trackpoint_get_time(src->tail.prev->trackp))
        {
            src->head.next->prev = &dest->head;
            dest->head.next->prev = src->tail.prev;
            src->tail.prev->next = dest->head.next;
            dest->head.next = src->head.next;
            trackpoint_destroy(src->head.trackp);
            trackpoint_destroy(src->tail.trackp);
            free(src);
            break;
        }
        // If the start time of dest is larger than the end time of src, just "add" them
        if (trackpoint_get_time(src->head.next->trackp) > trackpoint_get_time(dest->tail.prev->trackp))
        {
            dest->tail.prev->next = src->head.next;
            src->head.next->prev = dest->tail.prev;
            src->tail.prev->next = &dest->tail;
            dest->tail.prev = src->tail.prev;
            trackpoint_destroy(src->head.trackp);
            trackpoint_destroy(src->tail.trackp);
            free(src);
            break;
        }
        // If the src time of dest is larger than the end time of dest, just "add" them
        if (trackpoint_get_time(dest->head.next->trackp) > trackpoint_get_time(src->head.next->trackp))
        {
            currB = src->tail.prev;
            while (trackpoint_get_time(currB->trackp) >= trackpoint_get_time(dest->head.next->trackp))
            {
                k++;
                currB = currB->prev;
            }
            dest->head.next->prev = currB;
            dest->head.next = src->head.next;
            currB = currB->next;
            currB->prev->next = dest->head.next;
            dest->size = dest->size + src->size - k;
            currA = dest->head.next;

        }
        // Set pointers of both linked lists to the start point of the overlap if the starting point of overlap is the starting point in track A

        if (trackpoint_get_time(dest->head.next->trackp) < trackpoint_get_time(src->head.next->trackp))
        {
            currA = dest->tail.prev;
            while (trackpoint_get_time(currA->trackp) >= trackpoint_get_time(src->head.next->trackp))
            {
                currA = currA->prev;
            }
            currA = currA->next;
            currB = src->head.next;
        }
        // Set pointers of both linked lists to the start point of the overlap if the starting point of overlap is the starting point in track B

        while (currA != &dest->tail && currB != &src->tail)
        {
            if (((trackpoint_get_time(currB->trackp)) < (trackpoint_get_time(currA->trackp))) && ((trackpoint_get_time(currB->trackp)) > (trackpoint_get_time(currA->prev->trackp))))
            {
                currA->prev->next = currB;
                currB->prev = currA->prev;
                currA->prev = currB;
                currB = currB->next;
                currB->prev->next = currA;
                dest->size++;
                src->size--;
                continue;
                //Insert the currB unit into 2 units of trackA if its time is between these two
            }
            if (trackpoint_get_time(currB->trackp) == trackpoint_get_time(currA->trackp))
            {
                if (same_point(currB->trackp, currA->trackp))
                {
                    currB = currB->next;
                    trackpoint_destroy(currB->prev->trackp);
                    free(currB->prev);
                    src->size--;
                    continue;
                    //If two units in both tracks have the same time and same lat and lon, keep one of them
                }
                else
                {
                    currB = currB->next;
                    trackpoint_destroy(currB->prev->trackp);
                    free(currB->prev);
                    wrapoint* Atemp = currA;
                    currA->next->prev = currA->prev;
                    currA = currA->next;
                    currA->prev->next = currA;
                    trackpoint_destroy(Atemp->trackp);
                    free(Atemp);
                    dest->size--;
                    src->size--;
                    //If two units in both tracks have the same time and different lat and lon, discard both
                    continue;
                }
            }
            else
            {
                currA = currA->next;
                continue;
            }
        }
        if (currB == &src->tail)
        {
            trackpoint_destroy(src->head.trackp);
            trackpoint_destroy(src->tail.trackp);
            free(src);
        }
        if (currA == &dest->tail && currB != &src->tail)
        {
            currA->prev->next = currB;
            currB->prev = currA->prev;
            src->tail.prev->next = &dest->tail;
            dest->tail.prev = src->tail.prev;
            src->head.next = &src->tail;
            src->tail.prev = &src->head;
            dest->size = dest->size + src->size;
            track_destroy(src);
            // If there are remaining units in trackB, add all of them to trackA(dest)
        }
        break;
    }
}

double track_closest_approach(const track* track1, const track* track2)
{
    double intersect_upper;
    double intersect_lower;
    wrapoint* currA = track1->head.next;
    wrapoint* currB = track2->head.next;
    if (trackpoint_get_time(track1->head.next->trackp) > trackpoint_get_time(track2->head.next->trackp))
        intersect_lower = trackpoint_get_time(track1->head.next->trackp);
    if (trackpoint_get_time(track1->head.next->trackp) <= trackpoint_get_time(track2->head.next->trackp))
        intersect_lower = trackpoint_get_time(track2->head.next->trackp);
    if (trackpoint_get_time(track1->tail.prev->trackp) < trackpoint_get_time(track2->tail.prev->trackp))
        intersect_upper = trackpoint_get_time(track1->tail.prev->trackp);
    if (trackpoint_get_time(track1->tail.prev->trackp) >= trackpoint_get_time(track2->tail.prev->trackp))
        intersect_upper = trackpoint_get_time(track2->tail.prev->trackp);
    //set the start and end of overlap of two tracks
    double* timestamps0 = malloc(sizeof(double) * ((track1->size) + (track2->size)));
    int i = 0;
    while (trackpoint_get_time(currB->trackp) != 0 && trackpoint_get_time(currA->trackp) != 0)
    {
        if (trackpoint_get_time(currA->trackp) < trackpoint_get_time(currB->trackp))
        {
            timestamps0[i] = trackpoint_get_time(currA->trackp);
            i = i + 1;
            currA = currA->next;
            continue;
        }
        if (trackpoint_get_time(currB->trackp) < trackpoint_get_time(currA->trackp))
        {
            timestamps0[i] = trackpoint_get_time(currB->trackp);
            i = i + 1;
            currB = currB->next;
            continue;
        }
        if (trackpoint_get_time(currB->trackp) == trackpoint_get_time(currA->trackp))
        {
            timestamps0[i] = trackpoint_get_time(currB->trackp);
            i = i + 1;
            currB = currB->next;
            currA = currA->next;
            continue;
        }
    }
    //get the timestamps of all trackpoints in both tracks combined
    int super = 0;
    for (int j = 0; j < i; j++)
    {
        if (timestamps0[j] < intersect_lower)
            continue;
        super++;
    }
    double* timestamps = (double*)malloc(sizeof(double) * (super));
    int s = 0;
    for (int j = 0; j < i; j++)
    {
        if (timestamps0[j] < intersect_lower)
        {
            continue;
        }
        timestamps[s] = timestamps0[j];
        s++;
    }
    free(timestamps0);
    //eliminate those timestamps not between the start and end of both tracks and add them into a new array
    location** locA = malloc(sizeof(location*) * (i + 1));
    s = 0;
    currA = track1->head.next;
    double temp_lat = 0;
    double temp_lon = 0;
    while (trackpoint_get_time(currA->trackp) <= intersect_upper && trackpoint_get_time(currA->trackp) != 0)
    {
        if (s == super)
            break;
        if (trackpoint_get_time(currA->trackp) == timestamps[s])
        {
            temp_lat = trackpoint_get_latitude(currA->trackp);
            temp_lon = trackpoint_get_longitude(currA->trackp);
            locA[s] = location_create(temp_lat, temp_lon);
            s++;
            // if the given time point is exactly the time of a trackpoint, return the location
            continue;
        }
        if (trackpoint_get_time(currA->trackp) < timestamps[s] && trackpoint_get_time(currA->next->trackp) > timestamps[s])
        {
            temp_lat = linear(trackpoint_get_latitude(currA->trackp), trackpoint_get_latitude(currA->next->trackp), trackpoint_get_time(currA->next->trackp) - trackpoint_get_time(currA->trackp), timestamps[s] - trackpoint_get_time(currA->trackp));
            temp_lon = linear(trackpoint_get_longitude(currA->trackp), trackpoint_get_longitude(currA->next->trackp), trackpoint_get_time(currA->next->trackp) - trackpoint_get_time(currA->trackp), timestamps[s] - trackpoint_get_time(currA->trackp));
            locA[s] = location_create(temp_lat, temp_lon);
            s++;
            // if the given time point is in the time interval between 2 trackpoints, return the linear interpolated location
            continue;
        }
        else
        {
            currA = currA->next;
            continue;
        }
    }
    //get all locations in trackA corresponding to the time points
    location** locB = malloc(sizeof(location*) * (i + 1));
    s = 0;
    currB = track2->head.next;
    while (trackpoint_get_time(currB->trackp) <= intersect_upper && trackpoint_get_time(currB->trackp) != 0)
    {
        if (s == super)
            break;
        if (trackpoint_get_time(currB->trackp) == timestamps[s])
        {
            temp_lat = trackpoint_get_latitude(currB->trackp);
            temp_lon = trackpoint_get_longitude(currB->trackp);
            locB[s] = location_create(temp_lat, temp_lon);
            s++;
            continue;
        }
        if (trackpoint_get_time(currB->trackp) < timestamps[s] && trackpoint_get_time(currB->next->trackp) > timestamps[s])
        {
            temp_lat = linear(trackpoint_get_latitude(currB->trackp), trackpoint_get_latitude(currB->next->trackp), trackpoint_get_time(currB->next->trackp) - trackpoint_get_time(currB->trackp), timestamps[s] - trackpoint_get_time(currB->trackp));
            temp_lon = linear(trackpoint_get_longitude(currB->trackp), trackpoint_get_longitude(currB->next->trackp), trackpoint_get_time(currB->next->trackp) - trackpoint_get_time(currB->trackp), timestamps[s] - trackpoint_get_time(currB->trackp));
            locB[s] = location_create(temp_lat, temp_lon);
            s++;
            continue;
        }
        else
        {
            currB = currB->next;
        }
    }
    //get all locations in trackB corresponding to the time points
    double closest = INFINITY;
    for (int j = 0; j < s; j++)
    {
        double loc_dis = location_distance(locA[j], locB[j]);
        if (loc_dis < closest)
        {
            closest = loc_dis;
        }
    }
    //Find out the smallest one
    for (int j = 0; j < s; j++)
    {
        location_destroy(locA[j]);
    }
    free(locA);
    for (int j = 0; j < s; j++)
    {
        location_destroy(locB[j]);
    }
    free(locB);
    //destroy all locations in array locA and locB
    free(timestamps);
    return closest;
}