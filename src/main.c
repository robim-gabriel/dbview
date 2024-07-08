#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <unistd.h>

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

	while ((opt = getopt(argc, argv, "nf:")) != -1) {
		switch (opt) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
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
	
		// TODO: Still need to write to the file otherwise it will always fail to validate the header
		if (validate_db_header(dbfd, &db_header) == STATUS_ERROR) {
			printf("Failed to validate database header\n");
			return -1;
		}
	}

	close(dbfd);

	return 0;
}
