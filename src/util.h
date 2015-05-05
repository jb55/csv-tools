
#ifndef CSV_UTIL_H
#define CSV_UTIL_H

#include <stdio.h>

struct csv_parser;

typedef void (field_cb_fn)(void *, size_t, void *, int);
typedef void (row_cb_fn)(int, void *);

int
process_csv_file (field_cb_fn * field_cb,
                  row_cb_fn * row_cb,
                  struct csv_parser * parser,
                  const char * file);

int
process_csv_stream (field_cb_fn * field_cb,
                    row_cb_fn * row_cb,
                    struct csv_parser * parser,
                    FILE *stream);

int
streq (const char* str1, const char* str2);

#endif /* CSV_UTIL_H */
