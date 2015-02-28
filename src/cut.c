
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "csv.c/csv.h"

void usage (int status) {
  fputs ("Usage: csv-cut OPTION... [FILE]...\n", stdout);
  fputs ("Print selected fields from each csv FILE to standard output.\n\n", stdout);

  fputs ("Mandatory arguments to long options are mandatory for short options too.\n", stdout);
  fputs ("  -f, --fields=FIELD             select only these fields\n", stdout);
  fputs ("  -o, --output-delimiter=CHAR    select the output delimiters\n", stdout);
  fputs ("  -d, --input-delimiter=CHAR     select the input delimiters\n", stdout);
  fputs ("  -v, --complement               invert the field selection\n", stdout);
  fputs ("  -t, --output-tabs              shortcut for -o $'\\t'\n", stdout);

  exit (status);
};

// TODO (jb55): cut based on column names
static struct option const longopts[] =
{
  {"fields", required_argument, NULL, 'f'},
  {"output-delimiter", required_argument, NULL, 'o'},
  {"complement", no_argument, NULL, 'v'},
  {"output-tabs", no_argument, NULL, 't'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};


static int streq (const char* str1, const char* str2) {
  return strcmp (str1, str2) == 0;
}

static int chosen = -1;
static int complement = 0;
static char delim = ',';
static char output_delim = -1;
static int current_field = 0;

struct field_range {
  int start;
  int end;
};

void field_cb (void* data, size_t len, void* outfile, int is_last) {
  ++current_field;

  // TODO (jb55): branch prediction?
  // TODO (jb55): move field writing logic out of this utility
  // TODO (jb55): is_numeric field check to prevent quoting
  if (complement ? current_field != chosen : current_field == chosen) {
    if (!complement && output_delim == ',')
      csv_fwrite((FILE*) outfile, data, len);
    else
      fwrite(data, len, 1, (FILE*) outfile);

    if (complement && !is_last)
      fputc(output_delim, (FILE*) outfile);
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
      //error (0, errno, "%s", file);
      return 0;
    }
  }

  cut_csv_stream (stream);

  if (ferror (stream)) {
    //error (0, errno, "%s", file);
    return 0;
  }
  if (is_stdin) {
    clearerr (stream);
  }
  else if (fclose (stream) == EOF) {
    //error (0, errno, "%s", file);
    return 0;
  }

  return 1;
}

int main(int argc, const char *argv[]) {
  char c;
  int ok;

  while ((c = getopt_long (argc, argv, "d:f:o:vth", longopts, NULL)) != -1) {
    switch (c) {
      case 'f': {
        chosen = atoi(optarg);
        break;
      }
      case 'd': {
        delim = optarg[0];
        break;
      }
      case 'h': {
        usage(EXIT_SUCCESS);
        break;
      }
      case 'o': {
        output_delim = optarg[0];
        break;
      }
      case 't': {
        output_delim = '\t';
        break;
      }
      case 'v': {
        complement = 1;
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

  if (output_delim == -1) {
    output_delim = delim;
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
