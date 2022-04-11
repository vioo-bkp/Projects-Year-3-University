#include "so_stdio.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>

#define READ 1
#define WRITE 2
#define BUFFER_SIZE 4096

typedef struct _so_file {
	char *buffer;
	int file_d;
	int file_mode;
	int start;
	int end;
	int cursor_position;
	int operation;
	int mode;
	bool error;
	bool eof;
} SO_FILE;

SO_FILE *so_fopen(const char *pathname, const char *mode);
SO_FILE *so_popen(const char *command, const char *type);

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream);
size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream);
int min(int a, int b);
int so_feof(SO_FILE *stream);
int so_fgetc(SO_FILE *stream);
int so_fileno(SO_FILE *stream);
int so_ferror(SO_FILE *stream);
int so_fflush(SO_FILE *stream);
int so_fclose(SO_FILE *stream);
int so_pclose(SO_FILE *stream);
int so_fputc(int c, SO_FILE *stream);
int so_fseek(SO_FILE *stream, long offset, int whence);
long so_ftell(SO_FILE *stream);
