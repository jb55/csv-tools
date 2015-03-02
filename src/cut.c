
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "csv.c/csv.h"
#include "subcommands.h"

static void
usage (int status) {
  fputs ("Usage: csv-cut OPTION... [FILE]...\n", stdout);
  fputs ("Print selected fields from each csv FILE to standard output.\n\n", stdout);

  fputs ("Mandatory arguments to long options are mandatory for short options too.\n", stdout);
  fputs ("  -f, --fields=FIELD             select only these fields\n", stdout);
  fputs ("  -v, --complement               invert the field selection\n", stdout);

  exit (status);
};

// TODO (jb55): cut based on column names
static struct option const longopts[] =
{
  {"fields", required_argument, NULL, 'f'},
  {"complement", no_argument, NULL, 'v'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};


static int streq (const char* str1, const char* str2) {
  return strcmp (str1, str2) == 0;
}

static int chosen = -1;
static int complement = 0;
static char output_delim = -1;
static int current_field = 0;

struct field_range {
  int start;
  int end;
};

static void
field_cb (void* data, size_t len, void* outfile, int is_last) {
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

static void
row_cb(int c, void *outfile) {
  current_field = 0;
  fputc('\n', (FILE *) outfile);
}

static int
cut_csv_stream (struct csv_parser * parser, FILE *stream) {
  char buffer[65535];
  ssize_t nread;

  while ((nread = fread(buffer, 1, sizeof(buffer), stream)) > 0) {
    csv_parse(parser, buffer, nread, field_cb, row_cb, stdout);
  }

  csv_fini(parser, field_cb, row_cb, stdout);
}

// TODO (jb55): abstract this somehow?
static int
cut_csv_file (struct csv_parser * parser, const char * file) {
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

  cut_csv_stream (parser, stream);

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

// TODO (jb55): implement field range parsing, perhaps separate module?
struct field_range parse_field_range(const char *range) {
  int len = strlen(range);
  int i;
  for (i = 0; i < len; i++) {
  }
}

int cmd_cut(struct csv_parser * parser, extra_csv_opts_t *opts,
            int argc, const char ** argv) {
  char c;
  int ok;
  int i;

  output_delim = opts->output_delim;

  optind = 0;
  while ((c = getopt_long (argc, argv, "f:vh", longopts, NULL)) != -1) {
    switch (c) {
      case 'f': {
        chosen = atoi(optarg);
        break;
      }
      case 'h': {
        usage(EXIT_SUCCESS);
        break;
      }
      case 'v': {
        complement = 1;
        break;
      }
      default: {
        printf("csv-cut exiting because of %d\n", c);
        usage (EXIT_FAILURE);
      }
    }
  }

  if (optind == argc)
    ok = cut_csv_file (parser, "-");
  else {
    for (ok = 1; optind < argc; optind++)
      ok &= cut_csv_file (parser, argv[optind]);
  }

  return ok;
}
