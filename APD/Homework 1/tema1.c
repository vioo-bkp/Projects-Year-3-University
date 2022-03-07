// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
#include "genetic_algorithm.h"

void *thread_function(void *arg) {
    TThreadArgs thread_args = *(AThreadArgs) arg;
    int thread_id = thread_args.thread_id;
    int no_threads = thread_args.no_threads;
    AGeneticArgs genetic_info = thread_args.genetic_info;

    // sets the initial generation using parallelism
    int start, end;
    compute_thread_limits(genetic_info->object_count, thread_id, no_threads, &start, &end, NOT_SORT);
    set_initial_generation(thread_args.genetic_info, start, end);

    pthread_barrier_wait(thread_args.barrier);

    // run the parallel genetic algorithm
    run_genetic_algorithm(thread_args);

    pthread_barrier_wait(thread_args.barrier);

    // free resources for old generation
    free_generation(genetic_info->current_generation, thread_id, no_threads);
    free_generation(genetic_info->next_generation, thread_id, no_threads);

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int i, r, no_threads;

    // barrier
    pthread_barrier_t barrier;

    // genetic info
    AGeneticArgs genetic_args = (AGeneticArgs) calloc(1, sizeof(TGeneticArgs));
    if (!genetic_args) {
        printf("Calloc failure\n");
        exit(EXIT_FAILURE);
    }

    // read the input
    if (!read_input(&(genetic_args->objects), &(genetic_args->object_count), &(genetic_args->sack_capacity),
                    &(genetic_args->generations_count), argc, argv, &no_threads)) {
        return 0;
    }

    // initialise genetic vars
    genetic_args->current_generation = (individual *) calloc(genetic_args->object_count, sizeof(individual));
    if (!genetic_args->current_generation) {
        free(genetic_args);
        printf("Calloc failure\n");
        exit(EXIT_FAILURE);
    }

    genetic_args->next_generation = (individual *) calloc(genetic_args->object_count, sizeof(individual));
    if (!genetic_args->next_generation) {
        free(genetic_args->current_generation);
        free(genetic_args);
        printf("Calloc failure\n");
        exit(EXIT_FAILURE);
    }

    // threads
    pthread_t tid[no_threads];
    AThreadArgs thread_args = (AThreadArgs) calloc(no_threads, sizeof(TThreadArgs));
    if (!thread_args) {
        free(genetic_args->current_generation);
        free(genetic_args->next_generation);
        free(genetic_args);
        printf("Calloc failure\n");
        exit(EXIT_FAILURE);
    }

    // init barrier
    r = pthread_barrier_init(&barrier, NULL, no_threads);
    if (r) {
        printf("Failed to initialise the barrier\n");
        exit(EXIT_FAILURE);
    }

    // create the threads
    for (i = 0; i < no_threads; i++) {
        thread_args[i].thread_id = i;
        thread_args[i].genetic_info = genetic_args;
        thread_args[i].no_threads = no_threads;
        thread_args[i].barrier = &barrier;
        r = pthread_create(&tid[i], NULL, thread_function, &thread_args[i]);
        if (r) {
            printf("Failed to create the thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // joining the threads
    for (i = 0; i < no_threads; i++) {
        r = pthread_join(tid[i], NULL);

        if (r) {
            printf("Failed to join the thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // destroy the barrier
    r = pthread_barrier_destroy(&barrier);
    if (r) {
        printf("Failed to destroy the barrier\n");
        exit(EXIT_FAILURE);
    }

    // free the memory
    free(thread_args);
    free(genetic_args->current_generation);
    free(genetic_args->next_generation);
    free(genetic_args->objects);
    free(genetic_args);

    return 0;
}
