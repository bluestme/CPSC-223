all: GraphUnit Friends

GraphUnit:
        gcc -c graph.c
        gcc -c pqueue.c
        gcc -c graph_unit.c
        gcc -c string_key.c
        gcc -o GraphUnit graph.o pqueue.o string_key.o graph_unit.o
Friends:
        gcc -c graph.c
        gcc -c pqueue.c
        gcc -c string_key.c
        gcc -c friends.c
        gcc -o Friends graph.o pqueue.o string_key.o friends.o
