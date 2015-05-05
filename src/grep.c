

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "csv.c/csv.h"
#include "field-range-parser/field-range-parser.h"

#include "subcommands.h"
#include "inference.h"

static void
usage (int status) {
  fputs ("Usage: csv-grep OPTION... [FILE]...\n", stdout);
  fputs ("Grep on fields in a csv FILE to standard output.\n\n", stdout);

  fputs ("Mandatory arguments to long options are mandatory for short options too.\n", stdout);
  fputs ("  -f, --fields=FIELD             select only these fields\n", stdout);
  fputs ("  -v, --complement               invert the grep selection\n", stdout);

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

static int chosen = -1;
static int complement = 0;
static char output_delim = -1;
static char quote_char = '"';
static int current_field = 0;
static struct field_range frp;

static void
field_cb (void* data, size_t len, void* outfile, int is_last) {
  FILE * out = (FILE*)outfile;
  ++current_field;

  chosen = field_range_is_set(&frp, current_field);

  if (chosen) {
    if (output_delim != '\t' && should_quote((const char *)data, len, output_delim)) {
      fputc(quote_char, out);
      fwrite(data, len, 1, out);
      fputc(quote_char, out);
    }
    else
      fwrite(data, len, 1, out);

    if (!is_last)
      fputc(output_delim, out);
  }
}

static void
row_cb(int c, void *outfile) {
  current_field = 0;
  fputc('\n', (FILE *) outfile);
}

int cmd_grep(struct csv_parser * parser, extra_csv_opts_t *opts,
            int argc, const char ** argv) {
  char c;
  int ok, i;
  int parsed = 0;

  field_range_init(&frp, NULL);
  output_delim = opts->output_delim;
  quote_char = parser->quote_char;

  optind = 0;
  while ((c = getopt_long (argc, argv, "f:vh", longopts, NULL)) != -1) {
    switch (c) {
      case 'f': {
        parsed = 1;
        field_range_parse(&frp, optarg);
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
        usage (EXIT_FAILURE);
      }
    }
  }

  if (!parsed) {
    usage (EXIT_FAILURE);
  }

  if (optind == argc)
    ok = process_csv_file(parser, "-");
  else {
    for (ok = 1; optind < argc; optind++)
      ok &= process_csv_file(cut_csv_stream, parser, argv[optind]);
  }

  field_range_free(&frp, NULL);

  return ok;
}
