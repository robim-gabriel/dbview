#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x5254464D

struct dbheader_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

struct employee_t {
	unsigned int id;
	char name[MAX_LENGTH];
	char address[MAX_LENGTH];
	unsigned int hours;
};

int create_db_header(int fd, struct dbheader_t **header_out);
int validate_db_header(int fd, struct dbheader_t **header_out);
int output_file(int fd, struct dbheader_t *db_header, struct employee_t *employees);
int read_employees(int fd, struct dbheader_t *db_header, struct employee_t **employees_out);
int add_employee(struct dbheader_t *db_header, struct employee_t **employees, char *add_str);
int list_employees(struct dbheader_t *db_header, struct employee_t *employees);
int update_employee(struct dbheader_t *db_header, struct employee_t *employees, char* update_str);
int delete_employee(struct dbheader_t *db_header, struct employee_t **employees, char *remove_str);

#endif
