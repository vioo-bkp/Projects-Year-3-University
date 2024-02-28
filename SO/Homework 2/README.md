# Operating Systems - Homework 2: stdio Library

# Author
- Name: Mateescu Viorel-Cristian
- Group: 332CB

## Implementation

### so_fopen
- Opens a file using the specified `mode` permissions.
- Returns an allocated structure representing the opened file.
- Sets the cursor position to the beginning of the file.

### so_fclose
- Closes the file passed as a parameter and frees the memory allocated for the associated structure.

### so_popen
- Opens an allocated process from a structure.
- Creates a process with the command passed as a parameter.
- If the command is not valid, returns NULL.
- The child process redirects stdin (if we have write) and stdout (if we have read) to the corresponding file_descriptor and executes the given command.
- The parent process creates a structure associated with the child process and returns it.

### so_fileno
- Returns the file descriptor of the stream.

### so_fflush
- If the buffer is not empty, writes the contents of the buffer to the file with descriptor `file_d`, and sets the buffer position to 0.
- Returns 0 on success, -1 on error.

### so_ftell
- Returns the current position of the stream.

### so_ferror
- Returns 0 if no error occurred, otherwise returns 1.

### so_fgetc
- Returns the character at the current position of the stream and advances the position by +1.
- Clears the data in case of a previous write and sets the operation as read.

### so_fputc
- Writes the character at the current position of the stream and advances the position by +1.
- Clears the data in case of a previous write and sets the operation as write.
- Performs the write in case of an empty buffer.

### so_feof
- Returns 0 if the end of the file has not been reached, otherwise returns 1.

### so_fseek
- Moves the cursor to the specified position.
- Flushes the buffer if writes have been performed and resets the buffer index in case of reads.
- The cursor position can be relative to the beginning or end of the file, using lseek.

### so_fwrite
- Writes a specified number of bytes to the file.
- Returns the number of bytes actually written.

### so_fread
- Reads a specified number of bytes from the file.
- Returns the number of bytes actually read.
