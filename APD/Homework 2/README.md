# Homework 2 - Map-Reduce

## Author
- Name: Mateescu Viorel-Cristian
- Group: 332CB

## Implementation Details
- Implemented Replicated Workers model using ExecutorService for asynchronous and concurrent execution.
- Used RandomAccessFile to read a specific number of characters from a file at a given offset.
- Created DocumentMap class to store Map stage results for each document.
- Utilized List<DocumentMap> for storing Map and Reduce stage results.
- Calculated Fibonacci sequence up to (max_word_length_in_document + 1) term and stored it in an array for efficiency.
- Sorted the results of the Map-Reduce algorithm using an Entry Set based on document rank.
- Modularized the code into methods and made various optimizations.

## Implementation Steps
- Checked program execution and parsed input parameters.
- Initialized ExecutorService and created Map stage tasks by dividing the file into fragments.
- Waited for Map tasks to complete and created Reduce stage tasks using Map results.
- Sorted and displayed the results based on document rank.

- DocumentMap and MapReduceResult classes store Map and Reduce stage results.
- MapRunnable reads file fragments, generates word length dictionary, and calculates maximum word length.
- ReduceRunnable combines word length dictionaries, calculates document rank, and generates final results.
