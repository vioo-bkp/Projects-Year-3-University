# Homework 3 - Parallel Graph Algorithms

# Author:
- Name: Mateescu Viorel-Cristian
- Group: 332CB

# Implementation details:
- Reused file reading function from lab 10 and adapted it for reading the configuration of each Cluster by the Master process.
- Defined a group for communication between Master processes and a communicator specifically for Master processes.
- Used a fair task distribution formula for Master processes and Worker processes within a Cluster.
- Implemented custom scatter/gather methods for task distribution between Master processes and Worker processes.
- Modularized the code and made optimizations.

# Topology setup:
- Master processes read the cluster topology from a file and inform their Workers that they are their coordinators.
- Master processes broadcast the complete topology to each other and then to their Workers.

# Calculation process:
- Process 0 generates a vector v of size N, where v[i] = i.
- Process 0 distributes the tasks equally to each Cluster based on the number of Workers in each Cluster.
- Each Cluster's Master process distributes the assigned tasks fairly among its Workers.
- Worker processes solve the tasks and send the results back to their Masters.
- Masters collect the solved tasks in order from their Workers and send them to process 0.
- Process 0 collects the solved tasks from its own Workers and from other Clusters' Masters.
- Process 0 displays the resulting vector.
