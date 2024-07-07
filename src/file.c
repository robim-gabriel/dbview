#include <fcntl.h>
#include <stdio.h>

#include "file.h"
#include "common.h"

int create_db_file(char *filename) {
	int fd = open(filename, O_RDONLY);
	if (fd != -1) {
		printf("File already exists\n");
		return STATUS_ERROR;
	}

	fd = open(filename, O_CREAT | O_RDWR, 0644);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}

int open_db_file(char *filename) {
	int fd = open(filename, O_RDWR);
	if (fd == -1) {
		perror("open");
		return STATUS_ERROR;
	}

	return fd;
}
