
#include "util.h"

// TODO (jb55): abstract this somehow?
int
process_csv_file (field_cb_fn * field_cb,
                  row_cb_fn * row_cb,
                  struct csv_parser * parser,
                  const char * file) {
  FILE *stream;
  int is_stdin = streq(file, "-");

  if (is_stdin) {
    stream = stdin;
  }
  else {
    stream = fopen (file, "r");
    if (stream == NULL) {
      return 0;
    }
  }

  process_csv_stream(field_cb, row_cb, parser, stream);

  if (ferror (stream)) {
    return 0;
  }
  if (is_stdin) {
    clearerr (stream);
  }
  else if (fclose (stream) == EOF) {
    return 0;
  }

  return 1;
}

int streq (const char* str1, const char* str2) {
  return strcmp (str1, str2) == 0;
}


int
process_csv_stream (field_cb_fn * field_cb,
                    row_cb_fn * row_cb,
                    struct csv_parser * parser,
                    FILE *stream) {
  char buffer[65535];
  ssize_t nread;

  while ((nread = fread(buffer, 1, sizeof(buffer), stream)) > 0) {
    csv_parse(parser, buffer, nread, field_cb, row_cb, stdout);
  }

  csv_fini(parser, field_cb, row_cb, stdout);
}
