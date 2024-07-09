#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#include "common.h"
#include "parse.h"

int output_file(int fd, struct dbheader_t *db_header) {
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	db_header->magic = htonl(db_header->magic);
	db_header->filesize = htonl(db_header->filesize);
	db_header->version = htons(db_header->version);
	db_header->count = htons(db_header->count);

	if (lseek(fd, 0, SEEK_SET) == (off_t) -1 ) {
		perror("lseek");
		return STATUS_ERROR;
	}

	if (write(fd, db_header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("write");
		return STATUS_ERROR;
	}

	return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct dbheader_t **header_out) {
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	struct dbheader_t *db_header = calloc(1, sizeof(struct dbheader_t));
	if (db_header == NULL) {
		perror("calloc");
		return STATUS_ERROR;
	}

	if (read(fd, db_header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
		perror("read");
		free(db_header);
		return STATUS_ERROR;
	}

	db_header->version = ntohs(db_header->version);
	db_header->count = ntohs(db_header->count);
	db_header->magic = ntohl(db_header->magic);
	db_header->filesize = ntohl(db_header->filesize);

	if (db_header->version != 1) {
		printf("Version mismatch\n");
		free(db_header);
		return STATUS_ERROR;
	}

	if (db_header->magic != HEADER_MAGIC) {
		printf("File header does not match the expected format\n");
		free(db_header);
		return STATUS_ERROR;
	}

	struct stat db_stat = {0};
	fstat(fd, &db_stat);
	if (db_header->filesize != db_stat.st_size) {
		printf("Corrupted database\n");
		free(db_header);
		return STATUS_ERROR;
	}
	
	*header_out = db_header;

	return STATUS_SUCCESS;
}

int create_db_header(int fd, struct dbheader_t **header_out) {
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return STATUS_ERROR;
	}
	
	struct dbheader_t *db_header = calloc(1, sizeof(struct dbheader_t));
	if (db_header == NULL) {
		perror("calloc");
		return STATUS_ERROR;
	}

	db_header->magic = HEADER_MAGIC;
	db_header->version = 0x1;
	db_header->count = 0;
	db_header->filesize = sizeof(struct dbheader_t);	


	*header_out = db_header;
	return STATUS_SUCCESS;
}
