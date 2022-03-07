// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include "sack_object.h"
#include "individual.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defines for the computing the thread limits depending on the desired operation on the array
#define SORT 1
#define NOT_SORT 0

// genetic info structure
typedef struct {
    sack_object *objects;
    int object_count, generations_count, sack_capacity;

    individual *current_generation;
    individual *next_generation;
} *AGeneticArgs, TGeneticArgs;

// thread info structure
typedef struct {
    AGeneticArgs genetic_info;
    int thread_id;
    int no_threads;
    pthread_barrier_t *barrier;
} *AThreadArgs, TThreadArgs;

// reads input from a given file
int
read_input(sack_object **objects, int *object_count, int *sack_capacity, int *generations_count, int argc, char *argv[],
           int *no_threads);

// displays all the objects that can be placed in the sack
void print_objects(const sack_object *objects, int object_count);

// displays all or a part of the individuals in a generation
void print_generation(const individual *generation, int limit);

// displays the individual with the best fitness in a generation
void print_best_fitness(const individual *generation);

// computes the fitness function for each individual in a generation
void compute_fitness_function(TThreadArgs threadArgs);

// compares two individuals by fitness and then number of objects in the sack (to be used with qsort)
int cmpfunc(const void *a, const void *b);

// performs a variant of bit string mutation
void mutate_bit_string_1(const individual *ind, int generation_index);

// performs a different variant of bit string mutation
void mutate_bit_string_2(const individual *ind, int generation_index);

// performs one-point crossover
void crossover(individual *parent1, individual *child1, int generation_index);

// copies one individual
void copy_individual(const individual *from, const individual *to);

// deallocates a generation
void free_generation(individual *generation, int thread_id, int no_threads);

// runs the parallelized genetic algorithm
void run_genetic_algorithm(TThreadArgs thread_args);

// sets the initial generation using parallelism
void set_initial_generation(AGeneticArgs args, int start, int end);

// computes the minimum of two numbers
int min(int a, int b);

// swaps the next generation and the current generation
void swap_generations(individual **generation1, individual **generation2);

// computes a thread's section from an array - end and start indices
void compute_thread_limits(int n, int thread_id, int no_threads, int *start, int *end, int op);

// sorts the elements of the array using parallel merge sort
void parallel_merge_sort(TThreadArgs thread_args);

// merges two sub arrays of the source array
void merge(individual *source, int start, int mid, int end);

// classic merge-sort function
void merge_sort(individual *source, int start, int end);

// merges the sub arrays sorted by the threads
void merge_sub_arrays(AGeneticArgs genetic_info, int no_threads);

/* mutates first 20% children with the first version of bit string mutation and the next 20% children with the second
 * version of bit string mutation using parallelism */
void parallel_mutate_bit_string(TThreadArgs thread_args, int count, int cursor, int generation_index);

// keeps first 30% children (elite children selection) using parallelism
void parallel_copy_individual(TThreadArgs thread_args, int count);

#endif