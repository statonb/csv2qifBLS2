#ifndef _PARSE_CSV_LINE_H_
#define _PARSE_CSV_LINE_H_

#define MAX_LINE 4096
#define MAX_FIELDS  32

int parse_csv_line(const char *line, char fields[][MAX_LINE], int max_fields);
#endif