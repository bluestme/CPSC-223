#ifndef __UNDIGRAPH_H__
#define __UNDIGRAPH_H__

#include <stddef.h>
#include <stdbool.h>

struct undigraph;
typedef struct undigraph undigraph;

undigraph* undigraph_create();

/**
 * Destroy a provided graph
 *
 * @param g a pointer to an undigraph, non NULL
 *
 * Expected runtime: O(V + E)
 */
void undigraph_destroy(undigraph* g);

/**
 * Returns whether a vertex exists in the given graph
 *
 * @param g a pointer to an undigraph, non NULL
 * @param name the vertex in question
 * @return whether or not the vertex is present in g
 *
 * Expected runtime: O(1)
 */
bool undigraph_has_vertex(undigraph* g, char* name);

/**
 * Returns whether an edge exists in the given graph
 *
 * @param g a pointer to an undigraph, non NULL
 * @param a the first vertex of the edge to be added
 * @param b the second vertex of the edge to be added
 * @return whether or not the edge is present in g
 *
 * Expected runtime: O(1)
 */
bool undigraph_has_edge(undigraph* g, char* a, char* b);

/**
 * Adds an edge to a given graph.
 *
 * @param g a pointer to an undigraph, non NULL
 * @param a the first vertex of the edge to be added
 * @param b the second vertex of the edge to be added
 * @return whether or not the addition was succcessful
 *
 * If the edge a, b is already present in g, the return
 * value should be false. Vertices a and b may or may not
 * already be present in g.
 *  *
 * @param g a pointer to an undigraph, non NULL
 * @param a the first vertex of the edge to be added
 * @param b the second vertex of the edge to be added
 * @return whether or not the addition was succcessful
 *
 * If the edge a, b is already present in g, the return
 * value should be false. Vertices a and b may or may not
 * already be present in g.
 *
 * Expected runtime: O(1)
 */
bool undigraph_add_edge(undigraph* g, char* a, char* b);

/**
 * Returns the length of the shortest minimum path
 * between two vertices in the supplied graph.
 *
 * @param g a pointer to an undigraph, non NULL
 * @param start the first vertex of the query
 * @param target the second vertex of the query
 * @return the length of the shortest minimum path, or
 * 0 if there does not exist a path between the two vertices.
 *
 * The parameters start and target cannot be equal as determined by strcmp, and both start
 * and target are guaranteed to be present in the graph.
 *
 * Expected runtime: O(V + E)
 */
size_t undigraph_bfs_distance(undigraph* g, char* start, char* target);

/**
 * Returns all common neighbors to two vertices in the supplied graph
 *
 * @param g a pointer to an undigraph, non NULL
 * @param a the first vertex of the query
 * @param b the second vertex of the query
 * @param count the number of neighbors common to both a and b, initialized to 0 by the caller
 * @param successful_query the truthiness of whether or not the query succeeded
 * without any memory issues, initialized to false by the caller
 * @return an array of pointers to unsorted names, each of which having been copied.
 * Ownership of these copied names and the containing array is transferred to the caller.
 *
 * Both a and b are guaranteed to be present in g. NULL is returned in two cases:
 * 1) there was an allocation error or 2) there are no common neighbors.
 * If NULL is returned and successful_query is false, a memory issue occurred.
 * If NULL is returned otherwise, no memory issue occurred.
 *
 * Expected runtime: O(D' * S' + D'' * S''), although it is a precondition that no string
 * may have length greater than 20, collapsing the bound to O(D' + D'')
 */
char** undigraph_common_neighbors(undigraph* g, char* a, char* b, size_t* count, bool* is_valid_query);

#endif