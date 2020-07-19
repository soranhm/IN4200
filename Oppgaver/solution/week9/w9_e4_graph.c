// Reading a web graph.

#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "week9.h"


int main(int argc, char const *argv[]) {
    int N;
    if (argc < 2) {
        printf("Usage: ./w9_e4_graph.exe FILENAME N\n");
        return 1;
    } else if (argc == 3) {
        N = atoi(argv[2]);
    } else {
        N = 10;
    }
    printf("Finding %d top sites with inbound links.\n", N);

    webGraph graph;

    // Open file:
    FILE * fptr = fopen(argv[1], "r");
    if (fptr==NULL) {
        printf("Could not open file.\n");
        return 1;
    }

    // Handle the first four lines:
    printf("Parsing '%s'\n", argv[1]);

    fscanf(fptr, "%*[^\n]\n"); // Throw away the first line.
    fscanf(fptr, "%*[^\n]\n"); // and the second.

    fscanf(fptr, "%*c %*s %d %*s %d %*[^\n]\n", &graph.n_nodes, &graph.n_edges);
    printf("Nodes: %d, Edges: %d\n", graph.n_nodes, graph.n_edges);

    fscanf(fptr, "%*[^\n]\n"); // Throw away the fourth line.


    graph.in   = calloc(graph.n_nodes, sizeof *graph.in);
    graph.out  = calloc(graph.n_nodes, sizeof *graph.out);
    graph.self = calloc(graph.n_nodes, sizeof *graph.self);

    // Read the edge data.
    int from, to, self_links = 0;
    double read_start = omp_get_wtime();
    while (fscanf(fptr, "%d %d", &from, &to)!=EOF) {
        if (from == to) {
            graph.self[from]++;
            self_links++;
        } else {
            graph.out[from]++;
            graph.in[to]++;
        }
    }
    double read_end = omp_get_wtime();

    double read_tot = read_end - read_start;
    printf("Time reading file: %lfs\n", read_tot);
    printf("Number of self links: %d\n", self_links);


    // Selection algorithm:
    double select_start = omp_get_wtime();
    int *idx = calloc(N, sizeof *idx);
    heap_select(graph.in, graph.n_nodes, idx, N);
    double select_end = omp_get_wtime();

    double tot_select = select_end - select_start;
    printf("Time to find top %d: %.3lfms\n", N, 1000*tot_select);

    printf("Top linked:\n");
    for (size_t i = 0; i < N; i++)
    {
        printf("ID: %6d, Inbound: %5d\n", idx[i], graph.in[idx[i]]);
    }

    free(graph.in);
    free(graph.out);
    free(graph.self);

    fclose(fptr);

    return 0;
}
