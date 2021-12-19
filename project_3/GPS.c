#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "track.h"
#include "location.h"
#include "more_math.h"
#include "trackpoint.h"
void print_location(const trackpoint* tp, void* argv);

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "ERROR-- No arguments!\n");
        return 0;
        //No arguments condition
    }
    if (argc < 4)
    {
        fprintf(stderr, "ERROR-- Missing Filename!\n");
        return 0;
        // No input files cases
    }

    if (!fopen(argv[2], "r"))
    {
        fprintf(stderr, "ERROR-- File not found!\n");
        return 0;
    }
    if (!fopen(argv[3], "r"))
    {
        fprintf(stderr, "ERROR-- File not found!\n");
        return 0;
    }
    // files not found cases
    FILE* input1 = fopen(argv[2], "r");
    FILE* input2 = fopen(argv[3], "r");
    track* i1 = track_create();
    track* i2 = track_create();
    int break_pt = 0;
    double temp_lat = 0;
    double temp_lon = 0;
    double temp_time = 0;
    double temp1 = 0;
    double temp2 = 0;
    double temp3 = 0;
    location* loc_temp;
    trackpoint* track_temp;
    while (break_pt < 1)
    {
        if (fscanf(input1, "%lf %lf %lf", &temp_lat, &temp_lon, &temp_time) != EOF)
        {
            if (temp_time <= temp3)
            {
                fprintf(stderr, "ERROR!\n");
                return 0;
                //This has included: invalid longitude, end of EOF after longitude, missing timestamps cases since all three cases lead to the same result
            }
            if (temp_lat > 90 || temp_lat < -90)
            {
                fprintf(stderr, "ERROR!\n");
                return 0;
                //latitude out of range case
            }
            loc_temp = location_create(temp_lat, temp_lon);
            track_temp = trackpoint_create(loc_temp, temp_time);
            track_add_point(i1, track_temp);
            temp1 = temp_lat;
            temp2 = temp_lon;
            temp3 = temp_time;
            continue;
        }
        break;
    }
    // Read in first file trackpoints and add them in the track
    temp_lat = 0;
    temp_lon = 0;
    temp_time = 0;
    temp1 = 0;
    temp2 = 0;
    temp3 = 0;
    while (break_pt < 1)
    {
        if (fscanf(input2, "%lf %lf %lf", &temp_lat, &temp_lon, &temp_time) != EOF)
        {
            if (temp_time <= temp3)
            {
                fprintf(stderr, "ERROR!\n");
                return 0;
            }
            if (temp_lat > 90 || temp_lat < -90)
            {
                fprintf(stderr, "ERROR!\n");
                return 0;
            }
            loc_temp = location_create(temp_lat, temp_lon);
            track_temp = trackpoint_create(loc_temp, temp_time);
            track_add_point(i2, track_temp);
            continue;
        }
        break;
    }
    // Read in first second trackpoints and add them in the track
    if (strcmp(argv[1], "-closest") == 0)
    {
        printf("%.0lf\n", track_closest_approach(i1, i2));
        fclose(input1);
        fclose(input2);
        track_destroy(i1);
        track_destroy(i2);
        //calculate the closest distance
        return 0;
    }
    if (strcmp(argv[1], "-combine") == 0)
    {
        track_merge(i1, i2);
        const track* ii1 = i1;
        track_for_each(ii1, print_location, i1);
        //merge them and print all the lat and lon in the merged track
        fclose(input1);
        fclose(input2);
        track_destroy(i1);
        return 0;
    }
    return 0;
}

void print_location(const trackpoint* tp, void* argv)
{
    printf("%lf %lf %lf\n", trackpoint_get_latitude(tp), trackpoint_get_longitude(tp), trackpoint_get_time(tp));
    //print all lat and lon within a trackpoint
}