// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
#include "genetic_algorithm.h"

int min(int a, int b) {
    return a > b ? b : a;
}

void swap_generations(individual **generation1, individual **generation2) {
    individual *tmp = *generation1;
    *generation1 = *generation2;
    *generation2 = tmp;
}

void compute_thread_limits(int n, int thread_id, int no_threads, int *start, int *end, int op) {
    // computing the start using the formula from the First Laboratory
    *start = thread_id * (double) n / no_threads;

    if (op == NOT_SORT) {
        // computing the end with the formula from the First Laboratory
        *end = min((thread_id + 1) * (double) n / no_threads, n);
    } else if (op == SORT) {
        // computing the end with the formula from the Third Laboratory (end index for Merge Sort)
        *end = (thread_id + 1) * (double) n / no_threads - 1;
    }
}

int read_input(sack_object **objects, int *object_count, int *sack_capacity,
            int *generations_count, int argc, char *argv[], int *no_threads) {
    FILE *fp;

    if (argc < 4) {
        fprintf(stderr, "Usage:\n\t./tema1 in_file generations_count num_threads\n");
        return 0;
    }

    // read the number of threads
    *no_threads = atoi(argv[3]);

    fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }

    if (fscanf(fp, "%d %d", object_count, sack_capacity) < 2) {
        fclose(fp);
        return 0;
    }

    if (*object_count % 10) {
        fclose(fp);
        return 0;
    }

    sack_object *tmp_objects = (sack_object *) calloc(*object_count, sizeof(sack_object));

    for (int i = 0; i < *object_count; ++i) {
        if (fscanf(fp, "%d %d", &tmp_objects[i].profit, &tmp_objects[i].weight) < 2) {
            free(objects);
            fclose(fp);
            return 0;
        }
    }

    fclose(fp);

    *generations_count = (int) strtol(argv[2], NULL, 10);

    if (*generations_count == 0) {
        free(tmp_objects);

        return 0;
    }

    *objects = tmp_objects;

    return 1;
}

void print_objects(const sack_object *objects, int object_count) {
    for (int i = 0; i < object_count; ++i) {
        printf("%d %d\n", objects[i].weight, objects[i].profit);
    }
}

void print_generation(const individual *generation, int limit) {
    for (int i = 0; i < limit; ++i) {
        for (int j = 0; j < generation[i].chromosome_length; ++j) {
            printf("%d ", generation[i].chromosomes[j]);
        }

        printf("\n%d - %d\n", i, generation[i].fitness);
    }
}

void print_best_fitness(const individual *generation) {
    printf("%d\n", generation[0].fitness);
}

void compute_fitness_function(TThreadArgs thread_args) {
    int weight;
    int profit;
    int start, end;

    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // compute the limits for the current thread
    compute_thread_limits(genetic_info->object_count, thread_id, no_threads, &start, &end, NOT_SORT);

    // parallelize the outer for (better results are received this way, I've also tried parallelizing the inner one)
    for (int i = start; i < end; ++i) {
        weight = 0;
        profit = 0;
        for (int j = 0; j < genetic_info->current_generation[i].chromosome_length; ++j) {
            if (genetic_info->current_generation[i].chromosomes[j]) {
                weight += genetic_info->objects[j].weight;
                profit += genetic_info->objects[j].profit;
            }
        }

        // also update each individual's index in this for to reduce the number of loops
        genetic_info->current_generation[i].index = i;
        genetic_info->current_generation[i].fitness = (weight <= genetic_info->sack_capacity) ? profit : 0;
    }
}

int cmpfunc(const void *a, const void *b) {
    int i;
    individual *first = (individual *) a;
    individual *second = (individual *) b;

    int res = second->fitness - first->fitness; // decreasing by fitness
    if (res == 0) {
        int first_count = 0, second_count = 0;

        for (i = 0; i < first->chromosome_length && i < second->chromosome_length; ++i) {
            first_count += first->chromosomes[i];
            second_count += second->chromosomes[i];
        }

        res = first_count - second_count; // increasing by number of objects in the sack
        if (res == 0) {
            return second->index - first->index;
        }
    }

    return res;
}

void mutate_bit_string_1(const individual *ind, int generation_index) {
    int i, mutation_size;
    int step = 1 + generation_index % (ind->chromosome_length - 2);

    if (ind->index % 2 == 0) {
        // for even-indexed individuals, mutate the first 40% chromosomes by a given step
        mutation_size = ind->chromosome_length * 4 / 10;
        for (i = 0; i < mutation_size; i += step) {
            ind->chromosomes[i] = 1 - ind->chromosomes[i];
        }
    } else {
        // for even-indexed individuals, mutate the last 80% chromosomes by a given step
        mutation_size = ind->chromosome_length * 8 / 10;
        for (i = ind->chromosome_length - mutation_size; i < ind->chromosome_length; i += step) {
            ind->chromosomes[i] = 1 - ind->chromosomes[i];
        }
    }
}

void mutate_bit_string_2(const individual *ind, int generation_index) {
    int step = 1 + generation_index % (ind->chromosome_length - 2);

    // mutate all chromosomes by a given step
    for (int i = 0; i < ind->chromosome_length; i += step) {
        ind->chromosomes[i] = 1 - ind->chromosomes[i];
    }
}

void crossover(individual *parent1, individual *child1, int generation_index) {
    individual *parent2 = parent1 + 1;
    individual *child2 = child1 + 1;
    int count = 1 + generation_index % parent1->chromosome_length;

    memcpy(child1->chromosomes, parent1->chromosomes, count * sizeof(int));
    memcpy(child1->chromosomes + count, parent2->chromosomes + count,
           (parent1->chromosome_length - count) * sizeof(int));

    memcpy(child2->chromosomes, parent2->chromosomes, count * sizeof(int));
    memcpy(child2->chromosomes + count, parent1->chromosomes + count,
           (parent1->chromosome_length - count) * sizeof(int));
}

void copy_individual(const individual *from, const individual *to) {
    memcpy(to->chromosomes, from->chromosomes, from->chromosome_length * sizeof(int));
}

void free_generation(individual *generation, int thread_id, int no_threads) {
    int i, start, end;
    compute_thread_limits(generation->chromosome_length, thread_id, no_threads, &start, &end, NOT_SORT);

    for (i = start; i < end; ++i) {
        free(generation[i].chromosomes);
        generation[i].chromosomes = NULL;
        generation[i].fitness = 0;
    }
}

void set_initial_generation(AGeneticArgs args, int start, int end) {
    // set initial generation (composed of object_count individuals with a single item in the sack)
    for (int i = start; i < end; ++i) {
        args->current_generation[i].fitness = 0;
        args->current_generation[i].chromosomes = (int *) calloc(args->object_count, sizeof(int));
        args->current_generation[i].chromosomes[i] = 1;
        args->current_generation[i].index = i;
        args->current_generation[i].chromosome_length = args->object_count;

        args->next_generation[i].fitness = 0;
        args->next_generation[i].chromosomes = (int *) calloc(args->object_count, sizeof(int));
        args->next_generation[i].index = i;
        args->next_generation[i].chromosome_length = args->object_count;
    }
}

void merge(individual *source, int start, int mid, int end) {
    // start of the first and second array
    int iA = start;
    int iB = mid + 1;
    int i;

    // for storing the merged array
    individual *destination = (individual *) calloc(end - start + 1, sizeof(individual));

    // execute the operations on the thread's part of the array
    for (i = start; i <= end; i++) {
        if (end + 1 == iB || (iA <= mid && cmpfunc(&source[iA], &source[iB]) <= 0)) {
            /* if the iAth value from the first array is less than the iBth value from the second array or if there
             * aren't any values left in the second array */
            destination[i - start] = source[iA];
            iA++;
        } else {
            /* if the iBth value from the second array is less than the iAth value from the first array or if there
             * aren't any values left in the first array */
            destination[i - start] = source[iB];
            iB++;
        }
    }

    // copy the merged array from the destination to the source
    memcpy(source + start, destination, (end - start + 1) * sizeof(individual));
    free(destination);
}

void merge_sort(individual *source, int start, int end) {
    // check if the indices are valid
    if (start < end) {
        // compute middle index
        int middle = start + (end - start) / 2;
        // merge sort the first half of the array
        merge_sort(source, start, middle);
        // merge sort the second half of the array
        merge_sort(source, middle + 1, end);
        // merge the two halves
        merge(source, start, middle, end);
    }
}

void merge_sub_arrays(AGeneticArgs genetic_info, int no_threads) {
    // stores the index of the merged sub array (initially it is the first thread's end index)
    int merged_end = genetic_info->object_count / no_threads - 1, end;

    // merge the other sub arrays
    for (int i = 1; i < no_threads; ++i) {
        // end index of other thread's sub array
        end = (i + 1) * genetic_info->object_count / no_threads - 1;
        // merge it with the already merged sub array from it's left
        merge(genetic_info->current_generation, 0, merged_end, end);
        // update the index of the merged sub array
        merged_end = end;
    }
}

void parallel_merge_sort(TThreadArgs thread_args) {
    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // compute the limits for the current thread
    int start, end;
    compute_thread_limits(genetic_info->object_count, thread_id, no_threads, &start, &end, SORT);
    // sort the thread's part of the array
    merge_sort(genetic_info->current_generation, start, end);
}

void parallel_mutate_bit_string(TThreadArgs thread_args, int count, int cursor, int generation_index) {
    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // compute the limits for the current thread
    int start, end;
    compute_thread_limits(count, thread_id, no_threads, &start, &end, NOT_SORT);

    // execute the operations on the thread's part of the array
    for (int i = start; i < end; ++i) {
        // mutate first 20% children with the first version of bit string mutation
        copy_individual(genetic_info->current_generation + i, genetic_info->next_generation + cursor + i);
        mutate_bit_string_1(genetic_info->next_generation + cursor + i, generation_index);

        // mutate next 20% children with the second version of bit string mutation
        copy_individual(genetic_info->current_generation + i + count, genetic_info->next_generation + cursor + i);
        mutate_bit_string_2(genetic_info->next_generation + cursor + i, generation_index);
    }
}

void parallel_crossover(TThreadArgs thread_args, int count, int cursor, int generation_index) {
    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // compute the limits for the current thread
    int start, end;
    compute_thread_limits(count, thread_id, no_threads, &start, &end, NOT_SORT);

    // execute the operations on the thread's part of the array
    for (int i = start; i < end; i += 2) {
        // crossover first 30% parents with one-point crossover
        crossover(genetic_info->current_generation + i, genetic_info->next_generation + cursor + i,
                  generation_index);
    }
}

void parallel_copy_individual(TThreadArgs thread_args, int count) {
    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // compute the limits for the current thread
    int start, end;
    compute_thread_limits(count, thread_id, no_threads, &start, &end, NOT_SORT);

    // execute the operations on the thread's part of the array
    for (int i = start; i < end; ++i) {
        // keep first 30% children (elite children selection)
        copy_individual(genetic_info->current_generation + i, genetic_info->next_generation + i);
    }
}

void run_genetic_algorithm(TThreadArgs thread_args) {
    int count, cursor;
    individual *source, *destination;

    // extract thread info and genetic info from the args
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;
    pthread_barrier_t *barrier = thread_args.barrier;

    // iterate for each generation
    for (int k = 0; k < genetic_info->generations_count; ++k) {
        cursor = 0;

        // compute fitness and sort by it
        compute_fitness_function(thread_args);
        parallel_merge_sort(thread_args);

        pthread_barrier_wait(barrier);

        // merge the sorted sub arrays
        if (thread_id == 0) {
            merge_sub_arrays(genetic_info, no_threads);
        }

        pthread_barrier_wait(barrier);

        // keep first 30% children (elite children selection)
        count = genetic_info->object_count * 3 / 10;
        parallel_copy_individual(thread_args, count);
        cursor = count;

        /* mutates first 20% children with the first version of bit string mutation and the next 20% children with the
         * second version of bit string mutation */
        count = genetic_info->object_count * 2 / 10;
        parallel_mutate_bit_string(thread_args, count, cursor, k);
        cursor += 2 * count;

        pthread_barrier_wait(barrier);

        // crossover first 30% parents with one-point crossover
        count = genetic_info->object_count * 3 / 10 - 1;
        parallel_crossover(thread_args, count, cursor, k);

        pthread_barrier_wait(barrier);

        if (thread_id == 0) {
            // if there is an odd number of parents, the last one is kept as such
            if ((count + 1) % 2 == 1) {
                source = genetic_info->current_generation + genetic_info->object_count - 1;
                destination = genetic_info->next_generation + cursor + count;
                copy_individual(source, destination);
            }

            // switch to new generation
            swap_generations(&genetic_info->current_generation, &genetic_info->next_generation);

            if (k % 5 == 0) {
                print_best_fitness(genetic_info->current_generation);
            }
        }
        pthread_barrier_wait(barrier);
    }

    compute_fitness_function(thread_args);
    parallel_merge_sort(thread_args);

    pthread_barrier_wait(barrier);

    if (thread_id == 0) {
        merge_sub_arrays(genetic_info, no_threads);
        print_best_fitness(genetic_info->current_generation);
    }
}
