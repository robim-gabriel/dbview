#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x5254464D

struct dbheader_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

int create_db_header(int fd, struct dbheader_t **header_out);
int validate_db_header(int fd, struct dbheader_t **header_out);

#endif
