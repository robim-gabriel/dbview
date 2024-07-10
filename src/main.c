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
	char *addstr = NULL;
	bool list = false;

	while ((opt = getopt(argc, argv, "nf:a:l")) != -1) {
		switch (opt) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				addstr = optarg;
				break;
			case 'l':
				list = true;
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
	
	if (addstr) {
		if (add_employee(db_header, &employees, addstr) == STATUS_ERROR) {
			printf("Failed to add employee to database\n");
			return -1;
		}
	}

	if (list) {
		if (list_employees(db_header, employees) == STATUS_ERROR) {
			printf("Could not print employees\n");
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
