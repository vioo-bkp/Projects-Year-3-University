#include "tema2.h"

int min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

int so_fileno(SO_FILE *stream) { return stream->file_d; }
int so_feof(SO_FILE *stream) { return stream->eof ? SO_EOF : 0; }
int so_ferror(SO_FILE *stream) { return stream->error; }
long so_ftell(SO_FILE *stream) { return stream->cursor_position; }

SO_FILE *so_fopen(const char *pathname, const char *mode)
{
	SO_FILE *stream = NULL;

	stream = malloc(sizeof(SO_FILE));
	stream->operation = 0;
	stream->start = 0;
	stream->end = 0;
	stream->cursor_position = 0;
	stream->eof = false;
	stream->buffer = malloc(sizeof(char) * BUFFER_SIZE);
	stream->error = false;
	stream->mode = 0644;

	if (strcmp("r", mode) == 0)
		stream->file_mode = O_RDONLY;
	else if (strcmp("r+", mode) == 0)
		stream->file_mode = O_RDWR;
	else if (strcmp("w", mode) == 0)
		stream->file_mode = O_WRONLY | O_CREAT | O_TRUNC;
	else if (strcmp("w+", mode) == 0)
		stream->file_mode = O_RDWR | O_CREAT | O_TRUNC;
	else if (strcmp("a", mode) == 0)
		stream->file_mode = O_WRONLY | O_CREAT | O_APPEND;
	else if (strcmp("a+", mode) == 0)
		stream->file_mode = O_RDWR | O_CREAT | O_APPEND;
	else {
		free(stream->buffer);
		free(stream);
		return NULL;
	}

	stream->file_d = open(pathname, stream->file_mode, stream->mode);
	if (stream->file_d == SO_EOF) {
		free(stream->buffer);
		free(stream);
		return NULL;
	} else {
		return stream;
	}

}

int so_fflush(SO_FILE *stream)
{
	if (stream->operation == WRITE) {
		int bytes_written =
		write(stream->file_d, stream->buffer, stream->end - stream->start);
	if (bytes_written == SO_EOF) {
		stream->error = true;
		return SO_EOF;
	}
	stream->start = 0;
	stream->end = 0;
	stream->cursor_position += bytes_written;
	}
	return 0;

}

int so_fclose(SO_FILE *stream)
{
	int ret = false;

	if (stream->operation == WRITE)
		ret = so_fflush(stream);
	if (close(stream->file_d) || ret) {
		free(stream->buffer);
		free(stream);
		return SO_EOF;
	}
	free(stream->buffer);
	free(stream);
	return 0;

}

int so_fgetc(SO_FILE *stream)
{
	if (stream->operation == READ) {
		if (stream->cursor_position == stream->end) {
			stream->eof = true;
			return SO_EOF;
		}
		return stream->buffer[stream->cursor_position++];
	}
	stream->operation = READ;
	stream->start = 0;
	stream->end = 0;
	stream->cursor_position = 0;
	stream->eof = false;
	stream->error = false;
	int bytes_read = read(stream->file_d, stream->buffer, BUFFER_SIZE);

	if (bytes_read == SO_EOF) {
		stream->error = true;
		return SO_EOF;
	}
	stream->end = bytes_read;
	return stream->buffer[stream->cursor_position++];
}

int so_fseek(SO_FILE *stream, long offset, int whence)
{
	if (stream->operation == WRITE)
		so_fflush(stream);
	else if (stream->operation == READ)
		stream->start = stream->end = 0;

	stream->cursor_position = lseek(stream->file_d, offset, whence);

	if (stream->cursor_position == -1)
		return SO_EOF;
	else
		return 0;
}

int so_fputc(int c, SO_FILE *stream)
{
	if (stream->operation == WRITE) {
		if (stream->end != BUFFER_SIZE) {
			stream->buffer[stream->end++] = (char)c;
			return c;
		}
		so_fflush(stream);
	}
	stream->operation = WRITE;
	stream->buffer[stream->end++] = (char)c;
	return c;
}

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

	int bytes_to_write = size * nmemb;
	int bytes_written = 0;

	while (bytes_to_write > 0) {
		if (stream->operation == WRITE) {
			if (stream->end != BUFFER_SIZE) {
				int bytes_to_copy = min(bytes_to_write, BUFFER_SIZE - stream->end);

				memcpy(stream->buffer + stream->end, ptr + bytes_written,
							 bytes_to_copy);
				stream->end += bytes_to_copy;
				bytes_to_write -= bytes_to_copy;
				bytes_written += bytes_to_copy;
				continue;
			}
			so_fflush(stream);
		}
		stream->operation = WRITE;
		int bytes_to_copy = min(bytes_to_write, BUFFER_SIZE);

		memcpy(stream->buffer, ptr + bytes_written, bytes_to_copy);
		stream->end = bytes_to_copy;
		bytes_to_write -= bytes_to_copy;
		bytes_written += bytes_to_copy;
	}
	return bytes_written / size;

}

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{
	if (stream->operation == READ) {
		if (stream->cursor_position == stream->end) {
			stream->eof = true;
			return 0;
		}
		size_t bytes_read = min(size * nmemb, stream->end - stream->cursor_position);

		memcpy(ptr, stream->buffer + stream->cursor_position, bytes_read);
		stream->cursor_position += bytes_read;
		return bytes_read;
	}
	stream->operation = READ;
	stream->start = 0;
	stream->end = 0;
	stream->cursor_position = 0;
	stream->eof = false;
	stream->error = false;
	int bytes_read = read(stream->file_d, stream->buffer, BUFFER_SIZE);

	if (bytes_read == SO_EOF) {
		stream->error = true;
		return 0;
	}
	stream->end = bytes_read;
	size_t bytes_read_2 = min(size * nmemb, stream->end - stream->cursor_position);

	memcpy(ptr, stream->buffer + stream->cursor_position, bytes_read_2);
	stream->cursor_position += bytes_read_2;
	return bytes_read_2;

}

SO_FILE *so_popen(const char *command, const char *type)
{
	int pipe_fd[2];

	if (pipe(pipe_fd) == -1) {
		return NULL;
	} else if (pipe(pipe_fd) < 0)
		return NULL;
	pid_t pid = fork();

	if (pid == -1) {
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return NULL;
	} else if (pid == 0) {
		if (strcmp(type, "w") == 0) {
			close(pipe_fd[1]);
			if (pipe_fd[0] != STDIN_FILENO) {
				dup2(pipe_fd[0], STDIN_FILENO);
				close(pipe_fd[0]);
			}
		} else if (strcmp(type, "r") == 0) {
			close(pipe_fd[0]);
			if (pipe_fd[1] != STDOUT_FILENO) {
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(pipe_fd[1]);
			}
		} else {
			close(pipe_fd[0]);
			close(pipe_fd[1]);
			return NULL;
		}
		execl("/bin/sh", "sh", "-c", command, NULL);
		exit(0);
	} else {
		if (strcmp(type, "w") == 0)
			close(pipe_fd[0]);
		else if (strcmp(type, "r") == 0)
			close(pipe_fd[1]);
		else
			return NULL;
	}

	SO_FILE *stream = malloc(sizeof(SO_FILE));

	stream->file_d = pipe_fd[0];
	stream->buffer = malloc(BUFFER_SIZE);
	stream->start = 0;
	stream->end = 0;
	stream->cursor_position = 0;
	stream->operation = READ;
	stream->eof = false;
	stream->error = false;
	return stream;

}

int so_pclose(SO_FILE *stream)
{
	return 0;
}
