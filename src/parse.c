#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "common.h"
#include "parse.h"

int delete_employee(struct dbheader_t *db_header, struct employee_t **employees, char *remove_str) {
	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}
	
	if (db_header->count == 0) {
		printf("Empty database\n");
		return STATUS_ERROR;
	}
	
	if (*employees == NULL) {
		printf("NULL employee pointer\n");
		return STATUS_ERROR;
	}

	int i = 0;
	bool not_found = true;
	while (i < db_header->count) {
		if (strcmp((*employees)[i].name, remove_str) == 0) {
			not_found = false;
			break;
		}
		i++;
	}

	if (not_found) {
		printf("Employee not found\n");
		return STATUS_ERROR;
	}

	int new_count = db_header->count - 1;
	struct employee_t *cur_employees = calloc(new_count, sizeof(struct employee_t));
	if (cur_employees == NULL) {
		perror("calloc");
		return STATUS_ERROR;
	}

	int j = 0;
	int k = 0;
	for (; k < db_header->count; k++) {
		if (strcmp((*employees)[k].name, remove_str) != 0) {
			cur_employees[j].id = (*employees)[k].id;
			strcpy(cur_employees[j].name, (*employees)[k].name);
			strcpy(cur_employees[j].address, (*employees)[k].address);
			cur_employees[j++].hours = (*employees)[k].hours;
		}
	}

	free(*employees);
	*employees = cur_employees;
	db_header->count--;
	db_header->filesize -= sizeof(struct employee_t);
	return STATUS_SUCCESS;
}

int update_employee(struct dbheader_t *db_header, struct employee_t *employees, char *update_str) {
	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	if (db_header->count == 0) {
		printf("Empty database\n");
		return STATUS_ERROR;
	}
	
	if (employees == NULL) {
		printf("NULL employee pointer\n");
		return STATUS_ERROR;
	}

	char *name = strtok(update_str, ",");
	char *hours = strtok(NULL, ",");
	
	if (name == NULL || hours == NULL) {
		printf("Invalid update_str format\n");
		return STATUS_ERROR;
	}

	int i = 0;
	bool found = false;
	for (; i < db_header->count; i++) {
		if (strcmp(employees[i].name, name) == 0) {
			found = true;
			employees[i].hours = atoi(hours);
			break;
		}
	}

	if (found) {
		return STATUS_SUCCESS;
	} else {
		printf("Employee not found\n");
		return STATUS_ERROR;
	}
}

int list_employees(struct dbheader_t *db_header, struct employee_t *employees) {
	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	if (db_header->count == 0) {
		printf("Cannot print an empty database\n");
		return STATUS_ERROR;
	}
	
	if (employees == NULL) {
		printf("NULL employee pointer\n");
		return STATUS_ERROR;
	}

	int i = 0;
	for (; i < db_header->count; i++) {
		printf("Employee: %d\n", employees[i].id);
		printf("\tName: %s\n\tAddress: %s\n", employees[i].name, employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}

	return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *db_header, struct employee_t **employees, char *add_str) {
	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	char *name = strtok(add_str, ",");
	char *address = strtok(NULL, ",");
	char *hours = strtok(NULL, ",");
	
	db_header->count++;
	struct employee_t *updated_employees = calloc(db_header->count, sizeof(struct employee_t));
	if (updated_employees == NULL) {
		perror("calloc");
		return STATUS_ERROR;
	}

	if (*employees != NULL) {
		memcpy(updated_employees, *employees, (db_header->count - 1) * sizeof(struct employee_t));
		free(*employees);
	}

	updated_employees[db_header->count-1].id = time(NULL);
	strncpy(updated_employees[db_header->count-1].name, name, sizeof(updated_employees[db_header->count-1].name));
	strncpy(updated_employees[db_header->count-1].address, address, sizeof(updated_employees[db_header->count-1].address));
	updated_employees[db_header->count-1].hours = atoi(hours);
	
	*employees = updated_employees;
	db_header->filesize += sizeof(struct employee_t);

	return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *db_header, struct employee_t **employees_out) {
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	struct employee_t *cur_employees = calloc(db_header->count, sizeof(struct employee_t));
	if (cur_employees == NULL) {
		perror("calloc");
		return STATUS_ERROR;
	}

	if (read(fd, cur_employees, db_header->count * sizeof(struct employee_t)) != db_header->count * sizeof(struct employee_t)) {
		perror("read");
		free(cur_employees);
		return STATUS_ERROR;
	}

	int i = 0;
	for (; i < db_header->count; i++) {
		cur_employees[i].id = ntohl(cur_employees[i].id);
		cur_employees[i].hours = ntohl(cur_employees[i].hours);
	}

	*employees_out = cur_employees;

	return STATUS_SUCCESS;
}

int output_file(int fd, struct dbheader_t *db_header, struct employee_t *employees) {
	if (fd < 0) {
		printf("Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	if (db_header == NULL) {
		printf("NULL database header\n");
		return STATUS_ERROR;
	}

	/*
	 *	Eventualy, there will be deletions, which will lead
	 *	to a corrupted database if the last employee appended
	 *	isn't removed - because on deletion, every employee gets
	 *	shifted one position to the left - that's where ftruncate comes in
	 */	
	if (ftruncate(fd, db_header->filesize) == STATUS_ERROR) {
		perror("ftruncate");
		return STATUS_ERROR;
	}
	
	int count = db_header->count;

	db_header->magic = htonl(db_header->magic);
	db_header->filesize = htonl(sizeof(struct dbheader_t) + count * sizeof(struct employee_t));
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
	
	int i = 0;
	for (; i < count; i++) {
		employees[i].id = htonl(employees[i].id);
		employees[i].hours = htonl(employees[i].hours);
		if (write(fd, &employees[i], sizeof(struct employee_t)) != sizeof(struct employee_t)) {
			perror("write");
			return STATUS_ERROR;
		}
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
