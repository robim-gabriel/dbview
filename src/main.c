#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>

#include "file.h"
#include "common.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n: create new database file\n");
	printf("\t -f: (required) path to database\n");
	return;
}
	
int main(int argc, char *argv[]) {
	char *filepath = NULL;
	int opt;
	bool newfile = false;
	int dbfd = -1;
	struct dbheader_t *db_header = NULL;
	struct employee_t *employees = NULL;
	char *add_str = NULL;
	bool list = false;
	char *update_str = NULL;
	char *remove_str = NULL;

	while ((opt = getopt(argc, argv, "nf:a:lu:d:")) != -1) {
		switch (opt) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				add_str = optarg;
				break;
			case 'l':
				list = true;
				break;
			case 'u':
				update_str = optarg;
				break;
			case 'd':
				remove_str = optarg;
				break;
			case '?':
				printf("Unknown flag -%c\n", opt);
				break;
			default:
				return -1;
		}
	}

	if (filepath == NULL) {
		printf("Filepath is a required argument\n");
		print_usage(argv);
		return 0;
	}

	if (newfile) {
		dbfd = create_db_file(filepath);
		if (dbfd == STATUS_ERROR) { 
			printf("Failed to create database file\n"); 
			
			return -1;
		}
		
		if (create_db_header(dbfd, &db_header) == STATUS_ERROR) {
			printf("Failed to create database header\n");
			return -1;
		}
	} else {
		dbfd = open_db_file(filepath);
		if (dbfd == STATUS_ERROR) {
			printf("Unable to open database file\n");
			return -1;
		}
	
		if (validate_db_header(dbfd, &db_header) == STATUS_ERROR) {
			printf("Failed to validate database header\n");
			return -1;
		}
	}

	if (read_employees(dbfd, db_header, &employees) == STATUS_ERROR) {
		printf("Failed to read employees\n");
		return -1;
	}
	
	if (add_str) {
		if (add_employee(db_header, &employees, add_str) == STATUS_ERROR) {
			printf("Failed to add employee to database\n");
			free(db_header);
			free(employees);
			return -1;
		}
	}

	if (update_str) {
		if (update_employee(db_header, employees, update_str) == STATUS_ERROR) {
			printf("Unable to update employee\n");
			free(db_header);
			free(employees);
			return -1;
		}
	};

	if (list) {
		if (list_employees(db_header, employees) == STATUS_ERROR) {
			printf("Could not list employees\n");
			return -1;
		}
	}

	if (remove_str) {
		if (delete_employee(db_header, &employees, remove_str) == STATUS_ERROR) {
			printf("Failed to delete employee from database\n");
			free(db_header);
			free(employees);
			return -1;
		}
	}

	if (output_file(dbfd, db_header, employees) == STATUS_ERROR) {
		printf("Failed to write to database file descriptor\n");
		return -1;
	}

	if (close(dbfd) == STATUS_ERROR) {
		perror("close");
		return -1;
	}

	free(db_header);
	free(employees);

	return 0;
}
