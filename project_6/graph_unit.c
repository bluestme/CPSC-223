#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "graph.h"
#include "string_key.h"

/* for testing coupling between different maps */

void test_has_vertex();
void test_has_vertices(size_t n);
void test_has_edge();
void test_has_edge_reversed();
void test_add_edge();
void test_bfs_distance(size_t n);
void test_common_neighbors(size_t n);
void test_timing_bfs(size_t n, int on);

#define SMALL_TEST_SIZE 4
#define MEDIUM_SMALL_TEST_SIZE 20
#define MEDIUM_TEST_SIZE 1000
#define LARGE_TEST_SIZE 100000
#define VERY_LARGE_TEST_SIZE 1000000
#define PRINT_PASSED fprintf(stdout, "PASSED\n")
#define MAX_INT_DIGITS (20)

char** make_words(const char* prefix, size_t n)
{
    size_t prefix_len = strlen(prefix);
    char** arr = malloc(sizeof(char*) * n);
    for (size_t i = 0; i < n; i++)
    {
        arr[i] = malloc(sizeof(char) * (prefix_len + MAX_INT_DIGITS + 1));
        sprintf(arr[i], "%s%lu", prefix, i);
    }

    return arr;
}

void free_words(char** arr, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        free(arr[i]);
    }
    free(arr);
}

// these vars will be used for timing tests later
void foo(size_t n, int on) {
    return;
}

int main(int argc, char** argv)
{
    int test = 0;
    size_t n = 0;
    int on = 0;

    if (argc > 1)
    {
        test = atoi(argv[1]);
    }
    if (argc > 2)
    {
        // test size and on/off
        if (atoi(argv[2]) < 0)
        {
            fprintf(stderr, "%s: test size must be positive\n", argv[0]);
            return 1;
        }
        n = atoi(argv[2]);
        on = atoi(argv[3]) == 1;
        foo(n, on);
    }

    switch (test)
    {

    case 1:
        test_add_edge();
        break;

    case 2:
        test_has_vertex();
        break;

    case 3:
        test_has_edge();
        break;

    case 4:
        test_has_edge_reversed();
        break;

    case 5:
        test_has_vertices(MEDIUM_TEST_SIZE);
        break;

    case 6:
        test_bfs_distance(MEDIUM_SMALL_TEST_SIZE);
        break;

    case 7:
        test_common_neighbors(MEDIUM_SMALL_TEST_SIZE);
        break;
    case 8:
        test_timing_bfs(n, on);
        break;

    default:
        fprintf(stderr, "USAGE: %s test-number\n", argv[0]);
    }
}

void test_add_edge() {
    undigraph* g = undigraph_create();
    char person_1[] = "Bob";
    char person_2[] = "Joe";
    undigraph_add_edge(g, person_1, person_2);
    PRINT_PASSED;
    undigraph_destroy(g);
    return;
}

void test_has_vertex() {
    undigraph* g = undigraph_create();
    char person_1[] = "Bob";
    char person_2[] = "Joe";
    undigraph_add_edge(g, person_1, person_2);
    if (!undigraph_has_vertex(g, person_1) || !undigraph_has_vertex(g, person_2)) {
        printf("FAILED -- both adds not detected\n");
        goto cleanup;
    }
    PRINT_PASSED;
    undigraph_destroy(g);
    return;

cleanup:
    undigraph_destroy(g);
    return;
}

void test_has_edge() {
    undigraph* g = undigraph_create();
    char person_1[] = "Bob";
    char person_2[] = "Joe";
    undigraph_add_edge(g, person_1, person_2);
    if (!undigraph_has_edge(g, person_1, person_2)) {
        printf("FAILED -- edge not detected\n");
        goto cleanup;
    }
    PRINT_PASSED;
    undigraph_destroy(g);
    return;

cleanup:
    undigraph_destroy(g);
    return;
}
// has_edge should work even if order of vertices is reversed
void test_has_edge_reversed() {
    undigraph* g = undigraph_create();
    char person_1[] = "Bob";
    char person_2[] = "Joe";
    undigraph_add_edge(g, person_1, person_2);
    if (!undigraph_has_edge(g, person_2, person_1)) {
        printf("FAILED -- edge not detected\n");
        goto cleanup;
    }
    PRINT_PASSED;
    undigraph_destroy(g);
    return;

cleanup:
    undigraph_destroy(g);
    return;
}

void test_has_vertices(size_t n) {
    undigraph* g = undigraph_create();
    const char* prefix = "Bob";
    char** words = make_words(prefix, n);
    for (size_t i = 0; i < n - 1; i += 2) {
        undigraph_add_edge(g, words[i], words[i + 1]);
    }
    for (size_t i = 0; i < n - 1; i += 2) {
        if (!undigraph_has_edge(g, words[i], words[i + 1])) {
            printf("FAILED -- edge between %s and %s not detected\n", words[i], words[i + 1]);
            goto cleanup;
        }
    }
    undigraph_destroy(g);
    free_words(words, n);
    PRINT_PASSED;
    return;

cleanup:
    undigraph_destroy(g);
    free_words(words, n);
    return;
}

void test_bfs_distance(size_t n) {
    undigraph* g = undigraph_create();
    const char* prefix = "Bob";
    char** words = make_words(prefix, n);
    for (size_t i = 0; i < n - 1; i += 1) {
        undigraph_add_edge(g, words[i], words[i + 1]);
    }
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            size_t dist = undigraph_bfs_distance(g, words[i], words[j]);
            size_t real_dist = i > j ? i - j : j - i;

            if (dist != real_dist) {
                printf("FAILED -- distance beetween %s and %s outputted %ld instead of %ld\n",
                    words[i], words[j], dist, real_dist);
                goto cleanup;
            }
        }
    }
    PRINT_PASSED;
    undigraph_destroy(g);
    free_words(words, n);
    return;

cleanup:
    undigraph_destroy(g);
    free_words(words, n);
    return;
}

void test_common_neighbors(size_t n) {
    undigraph* g = undigraph_create();
    const char* prefix = "Bob";
    char** words = make_words(prefix, n);
    for (size_t i = 0; i < n - 1; i += 1) {
        undigraph_add_edge(g, words[i], words[i + 1]);
    }
    for (size_t i = 0; i < n - 1; i++) {
        for (size_t j = 0; j < n - 1; j++) {
            if (i == j) {
                continue;
            }
            size_t count = 0;
            bool success = false;
            char** name = undigraph_common_neighbors(g, words[i], words[j], &count, &success);
            size_t real_count = 0;
            if (i - j == 2 || j - i == 2)
                real_count = 1;
            if (count != real_count) {
                printf("FAILED -- common neighbors beetween %s and %s outputted %ld instead of %ld\n",
                    words[i], words[j], count, real_count);
                for (size_t k = 0; k < count; k++) {
                    free(name[k]);
                }
                free(name);
                goto cleanup;
            }
            if (count != 0) {
                size_t neighbor = i > j ? i - 1 : i + 1;
                if (strcmp(name[0], words[neighbor]) != 0) {
                    printf("FAILED -- common neighbors beetween %s and %s outputted %s instead of %s\n",
                        words[i], words[j], name[0], words[neighbor]);
                    for (size_t k = 0; k < count; k++) {
                        free(name[k]);
                    }
                    free(name);
                    goto cleanup;
                }
            }
            for (size_t k = 0; k < count; k++) {
                free(name[k]);
            }
            free(name);
        }
    }
    PRINT_PASSED;
    undigraph_destroy(g);
    free_words(words, n);
    return;

cleanup:
    undigraph_destroy(g);
    free_words(words, n);
    return;
}

void test_timing_bfs(size_t n, int on) {
    undigraph* g = undigraph_create();
    // make two linked lists
    char prefix_left[] = "left";
    char** words_left = make_words(prefix_left, n);
    for (size_t i = 0; i < n - 1; i++) {
        undigraph_add_edge(g, words_left[i], words_left[i + 1]);
    }
    char prefix_right[] = "right";
    char** words_right = make_words(prefix_right, n);
    for (size_t i = 0; i < n - 1; i++) {
        undigraph_add_edge(g, words_right[i], words_right[i + 1]);
    }
    // connect the two lists at top and bottom
    undigraph_add_edge(g, words_left[0], words_right[n - 1]);
    undigraph_add_edge(g, words_left[n - 1], words_right[0]);

    // call bfs on different sides of the circle
    if (on) {
        for (size_t i = 0; i < 10; i++) {
            size_t distance = undigraph_bfs_distance(g, words_left[i], words_right[i]);
            if (distance != n) {
                printf("FAILED -- distance between %s and %s is %zu instead of %zu\n", words_left[i], words_right[i], distance, n);
                goto cleanup;
            }
        }
    }
    undigraph_destroy(g);
    free_words(words_left, n);
    free_words(words_right, n);
    return;
    cleanup:
    undigraph_destroy(g);
    free_words(words_left, n);
    free_words(words_right, n);
    return;
}
