# Homework 1

## Author
- Name: Mateescu Viorel-Cristian
- Group: 332CB

## Implementation Details
- Parallelized for loops using formulas for dividing an array among threads.
- Synchronized threads using a barrier to ensure proper execution order.
- Combined mutation for loops into a single parallel loop.
- Optimized comparison function for sorting by pre-calculating a count value.
- Explored parallel sorting techniques.
- Used start and end indices to divide the array among threads for parallel sorting.
- Defined structures to store genetic algorithm information and thread arguments.
- Modularized the code by splitting the `run_genetic_algorithm` function into smaller functions.
- Encountered challenges with debugging threaded code and resolved synchronization issues.

## Conclusion
The code implements a parallel genetic algorithm. It divides the workload among threads, synchronizes their execution, and utilizes parallel sorting techniques. The implementation includes optimizations and modularization for improved readability and maintainability.
