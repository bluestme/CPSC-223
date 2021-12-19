all: Blotto GmapUnit

Blotto:
	gcc -c blotto.c
	gcc -c gmap.c
	gcc -c entry.c
	gcc -c string_key.c
	gcc -o Blotto string_key.o gmap.o entry.o blotto.o

GmapUnit:
                gcc -c gmap.c
                gcc -c entry.c
                gcc -c string_key.c
                gcc -c gmap_unit.c
                gcc -c gmap_test_functions.c
                gcc -o GmapUnit gmap.o entry.o string_key.o gmap_unit.o gmap_test_functions.o