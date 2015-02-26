
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "csv.c/csv.h"

static int streq (const char* str1, const char* str2) {
  return strcmp (str1, str2) == 0;
}

static int chosen = -1;
static char delim = ',';
static int current_field = 0;

struct field_range {
  int start;
  int end;
};

void field_cb (void* data, size_t len, void* outfile, int is_last) {
  if (++current_field == chosen) {
    fwrite(data, len, 1, (FILE*) outfile);
  }
}

void row_cb(int c, void *outfile) {
  current_field = 0;
  fputc('\n', (FILE *) outfile);
}

// TODO (jb55): implement field range parsing, perhaps separate module?
struct field_range parse_field_range(const char *range) {
  int len = strlen(range);
  int i;
  for (i = 0; i < len; i++) {
  }
}

static void
cut_csv_stream (FILE *stream) {
  char buffer[65535];
  struct csv_parser parser;
  ssize_t nread;

  csv_init(&parser, 0);
  csv_set_delim(&parser, delim);

  while ((nread = fread(buffer, 1, sizeof(buffer), stream)) > 0) {
    csv_parse(&parser, buffer, nread, field_cb, row_cb, stdout);
  }

  csv_fini(&parser, field_cb, row_cb, stdout);
  csv_free(&parser);
}

static int
cut_csv (char const *file) {
  FILE *stream;
  int is_stdin = streq(file, "-");

  if (is_stdin) {
    stream = stdin;
  }
  else {
    stream = fopen (file, "r");
    if (stream == NULL) {
      error (0, errno, "%s", file);
      return 0;
    }
  }

  cut_csv_stream (stream);

  if (ferror (stream)) {
    error (0, errno, "%s", file);
    return 0;
  }
  if (is_stdin) {
    clearerr (stream);
  }
  else if (fclose (stream) == EOF) {
    error (0, errno, "%s", file);
    return 0;
  }

  return 1;
}

void usage (int code) {
  printf("usage: csv-cut -f1 [FILES]\n");
  exit(code);
}

int main(int argc, const char *argv[]) {
  char c;
  int ok;

  while ((c = getopt (argc, argv, "f:d:")) != -1) {
    switch (c) {
      case 'f': {
        chosen = atoi(optarg);
        break;
      }
      case 'd': {
        delim = optarg[0];
        break;
      }
      case '?': {
        if (c == 'd') {
          delim = ',';
        }
        break;
      }
      default: {
        printf("exiting because of %d\n", c);
        usage (EXIT_FAILURE);
      }
    }
  }

  if (optind == argc)
    ok = cut_csv ("-");
  else {
    printf("file!?\n");
    for (ok = 1; optind < argc; optind++)
      ok &= cut_csv (argv[optind]);
  }

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
