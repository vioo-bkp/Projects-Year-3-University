#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Function to be executed by a thread 0 who will print the message
// "Thread x finished"
void *thread_function(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d finished\n", thread_id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int nr_threads;
    scanf("%d", &nr_threads);
    // define an array of seconds to be passed to the threads
    int seconds[nr_threads];
    for (int i = 1; i < nr_threads; i++) {
        scanf("%d", &seconds[i]);
    }
    // define an array of pthread_t to be used to store the threads
    pthread_t threads[nr_threads];
    // define an array of int to be used to store the thread ids
    int thread_ids[nr_threads];
    // create the threads
    for (int i = 1; i < nr_threads; i++) {
        thread_ids[i] = i;
        // sleep for the specified number of seconds
        sleep(seconds[i]);
        // create the thread
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);

        // join the threads
        pthread_join(threads[i], NULL);

    }

    return 0;
}
