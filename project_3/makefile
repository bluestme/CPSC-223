CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g3

GPS: GPS.o more_math.o location.o track.o trackpoint.o
	gcc -o $@ -g $^ -lm

GPS.o: track.h location.h more_math.h trackpoint.h
more_math.o: more_math.h
location.o: trackpoint.h
track.o: track.h location.h more_math.h trackpoint.h
trackpoint.o: trackpoint.h
