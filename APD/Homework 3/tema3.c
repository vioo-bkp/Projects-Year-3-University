// Nume: Mateescu Viorel-Cristian
// Grupa: 332CB
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_MASTERS 3

// Minimum function.
int min(int a, int b) {
    return a < b ? a : b;
}

// Function for reading a cluster's configuration from a file.
int *read_file(int rank, int *num_workers) {
    FILE *fp;
    char file_name[15];
    sprintf(file_name, "./cluster%d.txt", rank);

    // Open the file.
    fp = fopen(file_name, "r");
    if (fp == NULL) {
        exit(EXIT_FAILURE);
    }

    // Read the number of Workers in the current Cluster.
    fscanf(fp, "%d", num_workers);

    // Allocate memory and read the Workers indices in the current Cluster.
    int *workers = calloc(*num_workers, sizeof(int));
    if (workers == NULL) {
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < *num_workers; ++i) {
        fscanf(fp, "%d", &workers[i]);
    }

    return workers;
}

// Function for allocating memory for an array.
int *allocate_array(int N) {
    int *v = (int *) calloc(N, sizeof(int));
    if (!v) {
        exit(EXIT_FAILURE);
    }

    return v;
}

// Function for initialising the task array.
int *init_task_array(int N) {
    int *v = allocate_array(N);

    for (int i = 0; i < N; ++i) {
        v[i] = i;
    }

    return v;
}

// Function for printing the messages sent in a broadcast.
void print_broadcast_messages(int sender, int *receivers, int no_receivers) {
    for (int i = 0; i < no_receivers; ++i) {
        if (receivers[i] != sender) {
            printf("M(%d,%d)\n", sender, receivers[i]);
        }
    }
}

// Function for sending the Topology to the Workers.
void send_topology_to_workers(int rank, int *cluster_workers, int **topology) {
    for (int i = 0; i < cluster_workers[rank]; ++i) {
        // Send each Cluster's Configuration.
        for (int j = 0; j < NO_MASTERS; ++j) {
            // Send the number of Workers in the Cluster.
            MPI_Send(&cluster_workers[j], 1, MPI_INT, topology[rank][i], j, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, topology[rank][i]);

            // Send the Cluster's Topology.
            MPI_Send(topology[j], cluster_workers[j], MPI_INT, topology[rank][i], j, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, topology[rank][i]);
        }
    }
}

// Function for printing the Topology.
void print_topology(int rank, int **topology, int *cluster_workers) {
    printf("%d -> ", rank);

    for (int i = 0; i < NO_MASTERS; ++i) {
        printf("%d:", i);

        for (int j = 0; j < cluster_workers[i]; ++j) {
            printf("%d", topology[i][j]);

            if (j != cluster_workers[i] - 1) {
                printf(",");
            } else {
                printf(" ");
            }
        }
    }

    printf("\n");
}

// Function for receiving the Topology by a Worker from his Master.
void receive_topology(int master, int **topology, int *cluster_workers) {
    MPI_Status status;
    for (int i = 0; i < NO_MASTERS; ++i) {
        // Receive the number of workers in the current cluster and the processes in it.
        MPI_Recv(&cluster_workers[i], 1, MPI_INT, master, i, MPI_COMM_WORLD, &status);
        topology[i] = allocate_array(cluster_workers[i]);
        MPI_Recv(topology[i], cluster_workers[i], MPI_INT, master, i, MPI_COMM_WORLD, &status);
    }
}

// Function for completing the Topology by each Master Process.
void complete_topology(int rank, int **topology, int *cluster_workers, int *masters, MPI_Comm master_comm) {
    // Broadcast the number of workers in each Cluster.
    for (int i = 0; i < NO_MASTERS; ++i) {
        MPI_Bcast(&cluster_workers[i], 1, MPI_INT, i, master_comm);
    }
    print_broadcast_messages(rank, masters, NO_MASTERS);

    for (int i = 0; i < NO_MASTERS; ++i) {
        if (i != rank) {
            topology[i] = allocate_array(cluster_workers[i]);
        }
    }

    // Broadcast the Workers in each Cluster.
    for (int i = 0; i < NO_MASTERS; ++i) {
        MPI_Bcast(topology[i], cluster_workers[i], MPI_INT, i, master_comm);
    }
    print_broadcast_messages(rank, masters, NO_MASTERS);

    // Send the Topology to the Worker Processes in the Cluster. 
    send_topology_to_workers(rank, cluster_workers, topology);

    print_topology(rank, topology, cluster_workers);
}

// Function for scattering an array to Workers.
void scatter_tasks_to_workers(int rank, int no_workers, int *workers, int *tasks, int no_tasks) {
    int pos = 0, start, end, no_tasks_for_worker;
    for (int i = 0; i < no_workers; ++i) {
        // Compute the start and end indices for the Worker's subarray (also the number of tasks for him).
        start = i * (double) no_tasks / no_workers;
        end = min((i + 1) * (double) no_tasks / no_workers, no_tasks);
        no_tasks_for_worker = end - start;

        // Send the number of tasks and then the subarray of tasks to the Worker.
        MPI_Send(&no_tasks_for_worker, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, workers[i]);

        MPI_Send(tasks + pos, no_tasks_for_worker, MPI_INT, workers[i], 0, MPI_COMM_WORLD);
        printf("M(%d,%d)\n", rank, workers[i]);

        pos += no_tasks_for_worker;
    }
}

// Function for gathering an array from Workers.
void gather_tasks_from_workers(int rank, int no_workers, int *workers, int *tasks, int no_tasks) {
    MPI_Status status;

    int pos = 0, no_tasks_for_worker;
    for (int i = 0; i < no_workers; ++i) {
        // Receive the number of tasks the Worker has solved and then the solved tasks.
        MPI_Recv(&no_tasks_for_worker, 1, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status);
        MPI_Recv(tasks + pos, no_tasks_for_worker, MPI_INT, workers[i], 0, MPI_COMM_WORLD, &status);
        pos += no_tasks_for_worker;
    }
}

// Function for scattering an array to Masters.
int scatter_tasks_to_masters(int rank, int no_workers, int *cluster_workers, int **topology, int no_tasks,
                             int *tasks) {
    int pos = 0, cluster_0_tasks, tasks_for_cluster, start, end;
    for (int i = 0; i < NO_MASTERS; ++i) {
        // Compute the number of tasks for each Cluster according to the number of workers in each one.
        tasks_for_cluster = 0;
        for (int j = 0; j < cluster_workers[i]; j++) {
            start = (topology[i][j] - NO_MASTERS) * (double) no_tasks / no_workers;
            end = min((topology[i][j] - NO_MASTERS + 1) * (double) no_tasks / no_workers, no_tasks);

            tasks_for_cluster += end - start;
        }

        // Send the number of tasks and the tasks to each Cluster's Master Process.
        if (i != rank) {
            MPI_Send(&tasks_for_cluster, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, i);

            MPI_Send(tasks + pos, tasks_for_cluster, MPI_INT, i, 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n", rank, i);
        } else {
            cluster_0_tasks = tasks_for_cluster;
        }

        pos += tasks_for_cluster;
    }

    return cluster_0_tasks;
}

// Function for gathering an array from Masters.
void gather_tasks_from_masters(int *tasks) {
    MPI_Status status;

    int no_tasks, pos = 0;

    // Receive the number of tasks solved and the solved tasks from each Cluster.
    for (int i = 1; i < NO_MASTERS; ++i) {
        MPI_Recv(&no_tasks, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(tasks + pos, no_tasks, MPI_INT, i, 0, MPI_COMM_WORLD, &status);

        pos += no_tasks;
    }
}

// Function for coordinating each Cluster's work by the 0 Process.
void coordinate_masters(int no_processes, int rank, int **topology, int *cluster_workers, int *v, int N) {
    // Scatter the tasks to the other Masters in the Topology considering the number of Workers in each Cluster.
    int cluster_0_tasks = scatter_tasks_to_masters(rank, no_processes - NO_MASTERS, cluster_workers,
                                                   topology, N, v);

    // Scatter the Cluster's tasks to the Workers in the 0 Cluster and gather the results from them.
    scatter_tasks_to_workers(rank, cluster_workers[rank], topology[rank], v, cluster_0_tasks);
    gather_tasks_from_workers(rank, cluster_workers[rank], topology[rank], v, cluster_0_tasks);

    // Gather the results from the other Master Processes.
    gather_tasks_from_masters(v + cluster_0_tasks);
}

// Function for receiving the tasks from Process 0, splitting them to the Workers in the current Cluster, gathering 
// each Worker's results and then sending them to the Process 0. 
void coordinate_cluster(int rank, int **topology, int *cluster_workers) {
    MPI_Status status;
    int N;

    // Receive the number of tasks for the Cluster and then the tasks.
    MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    int *v = allocate_array(N);
    MPI_Recv(v, N, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    // Split them among the Workers and then Gather the results.
    scatter_tasks_to_workers(rank, cluster_workers[rank], topology[rank], v, N);
    gather_tasks_from_workers(rank, cluster_workers[rank], topology[rank], v, N);

    // Send the Results to the Process 0.
    MPI_Send(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, 0);

    MPI_Send(v, N, MPI_INT, 0, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, 0);

    free(v);
}

// Function for receiving the tasks from the Master Process, solving them and then sending the results back.
void solve_tasks(int rank, int master) {
    MPI_Status status;
    int N;

    // Receive the number of tasks and then the tasks.
    MPI_Recv(&N, 1, MPI_INT, master, 0, MPI_COMM_WORLD, &status);
    int *v = allocate_array(N);
    MPI_Recv(v, N, MPI_INT, master, 0, MPI_COMM_WORLD, &status);

    // Solve the Tasks.
    for (int i = 0; i < N; ++i) {
        v[i] *= 2;
    }

    // Send the Results back to the Master.
    MPI_Send(&N, 1, MPI_INT, master, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, master);

    MPI_Send(v, N, MPI_INT, master, 0, MPI_COMM_WORLD);
    printf("M(%d,%d)\n", rank, master);

    free(v);
}

// Free the allocated memory.
void free_topology(int **topology) {
    for(int i = 0; i < NO_MASTERS; ++i) {
        free(topology[i]);
    }
}

// Function for printing the Result Array.
void print_results(int N, int *v) {
    printf("Rezultat: ");

    for (int i = 0; i < N; ++i) {
        printf("%d ", v[i]);
    }

    printf("\n");
}

int main(int argc, char *argv[]) {
    int no_processes, rank;
    MPI_Status status;

    int master, *topology[NO_MASTERS], cluster_workers[NO_MASTERS], *v, N;

    MPI_Init(&argc, &argv);

    // Total number of processes.
    MPI_Comm_size(MPI_COMM_WORLD, &no_processes);

    // The current process ID / Rank.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the group of processes in MPI_COMM_WORLD.
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);

    // The processes in the Master group/communicator.
    int masters[NO_MASTERS] = {0, 1, 2};

    // Create the Master Group.
    MPI_Group master_group;
    MPI_Group_incl(world_group, NO_MASTERS, masters, &master_group);

    // Create a new communicator based on the Master Group.
    MPI_Comm master_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, master_group, 0, &master_comm);

    // Master Process.
    if (rank < NO_MASTERS) {
        // Read the Workers in the current Cluster.
        topology[rank] = read_file(rank, &(cluster_workers[rank]));

        // Notify the Workers about their Master.
        for (int i = 0; i < cluster_workers[rank]; ++i) {
            MPI_Send(&rank, 1, MPI_INT, topology[rank][i], 0, MPI_COMM_WORLD);
        }

        // Complete the Topology with the other Clusters' configurations.
        complete_topology(rank, topology, cluster_workers, masters, master_comm);
    } else {
        // Receive the Master.
        MPI_Recv(&master, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        // Receive and print the Topology.
        receive_topology(master, topology, cluster_workers);
        print_topology(rank, topology, cluster_workers);
    }

    if (rank == 0) {
        // Initialise the task array.
        N = atoi(argv[1]);
        v = init_task_array(N);

        // Solve the tasks using each clusters workforce.
        coordinate_masters(no_processes, rank, topology, cluster_workers, v, N);
    } else if (rank == 1 || rank == 2) {
        // Coordinate the workers in the current cluster.
        coordinate_cluster(rank, topology, cluster_workers);
    } else {
        // Receive the tasks, solve them and send the results to the Master Process.
        solve_tasks(rank, master);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    // Print the Results.
    if (rank == 0) {
        print_results(N, v);
        free(v);
    }

    free_topology(topology);

    MPI_Finalize();
}
