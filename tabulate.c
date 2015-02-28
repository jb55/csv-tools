
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "csv.c/csv.h"

int chosenCol = 0;

void field_cb(void* data, size_t len, void* outfile, int is_last) {
  fwrite(data, len, 1, (FILE*)outfile);
  if (!is_last)
    fputc('\t', (FILE*)outfile);
}

void row_cb(int c, void *outfile) {
  fputc('\n', (FILE *)outfile);
}

int main(int argc, const char *argv[]) {
  char buffer[65535];
  ssize_t nread;
  struct csv_parser parser;

  csv_init(&parser, 0);
  
  while ((nread = fread(buffer, 1, sizeof(buffer), stdin)) > 0) {
    csv_parse(&parser, buffer, nread, field_cb, row_cb, stdout);
  }

  csv_fini(&parser, field_cb, row_cb, stdout);
  csv_free(&parser);

  return 0;
}
