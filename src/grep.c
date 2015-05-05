

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

static char *to_match;
static int matched = 0;
static int complement = 0;
static char output_delim = -1;
static char quote_char = '"';
static int current_field = 0;
static struct field_range frp;

static char tmp_buf[65535];
static int pos = 0;

static void
field_cb (void* data, size_t len, void* outfile, int is_last) {
  FILE * out = (FILE*)outfile;
  ++current_field;

  matched = memcmp(to_match, data, len) == 0;

  // TODO: refactor this logic
  if (output_delim != '\t' && should_quote((const char *)data, len, output_delim)) {
    tmp_buf[pos++] = quote_char;
    memcpy(&tmp_buf[pos], data, len);
    pos += len;
    tmp_buf[pos++] = quote_char;
  }
  else {
    memcpy(&tmp_buf[pos], data, len);
    pos += len;
  }

  if (!is_last)
    tmp_buf[pos++] = output_delim;
}

static void
row_cb(int c, void *outfile) {
  FILE * out = (FILE*)outfile;

  if (matched) {
    fwrite(tmp_buf, pos, 1, out);
  }

  pos = 0;
  current_field = 0;
  matched = 0;

  fputc('\n', out);
}


int cmd_grep(struct csv_parser * parser, extra_csv_opts_t *opts, int argc,
             const char ** argv) {
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

  to_match = argv[optind];

  if (optind == argc)
    ok = process_csv_file(field_cb, row_cb, parser, "-");
  else {
    for (ok = 1; optind < argc; optind++)
      ok &= process_csv_file(field_cb, row_cb, parser, argv[optind]);
  }

  field_range_free(&frp, NULL);

  return ok;
}
