#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

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
		printf("Handle file creation here\n");
	} else {
		printf("Open file if it already exists\n");
	}

	printf("Close file descriptor opened and/or created\n");

	return 0;
}
