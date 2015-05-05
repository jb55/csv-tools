

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "csv.c/csv.h"
#include "subcommands.h"

void usage (int status) {
  fputs ("Usage: csv [OPTION]... <subcommand> [COMMAND_OPTION]... [FILE]...\n", stdout);
  fputs ("Print selected fields from each csv FILE to standard output.\n\n", stdout);

  fputs ("csv base options\n\n", stdout);
  fputs ("  -d, --input-delimiter=CHAR     select the input delimiter\n", stdout);
  fputs ("  -o, --output-delimiter=CHAR    select the output delimiter\n", stdout);
  fputs ("  -t, --output-tabs              use a tab output delimiter\n", stdout);
  fputs ("  -q, --quote-char=CHAR          quote", stdout);

  fputs ("\n\ncsv subcommands\n", stdout);
  fputs ("  cut                            cut columns\n", stdout);

  exit (status);
};

static struct option const longopts[] =
{
  {"output-delimiter", required_argument, NULL, 'o'},
  {"input-delimiter", required_argument, NULL, 'd'},
  {"quote-char", required_argument, NULL, 'q'},
  {"output-tabs", no_argument, NULL, 't'},
  {"help", no_argument, NULL, 'h'},
  {NULL, 0, NULL, 0}
};

static char delim = ',';
static int current_field = 0;

struct field_range {
  int start;
  int end;
};

typedef struct cmd_t {
	const char *cmd;
	int (*fn)(struct csv_parser *, extra_csv_opts_t *, int, const char **);
} cmd_t;

static cmd_t commands[] = {
	//{ "id", cmd_id },
	{ "cut", cmd_cut }
};

// TODO (jb55): move me somewhere nicer
#define NELEMS(x)  (sizeof(x) / sizeof(x[0]))

// check to see if a command is in the subcommand list
static cmd_t *
get_cmd (const char * cmd) {
  int i;
  cmd_t * command = NULL;

  for (i = 0; i < NELEMS(commands); i++) {
    command = &commands[i];
    if (streq(command->cmd, cmd))
      return command;
  }

  return NULL;
}

// find a subcommand from a list of strings
static int
find_subcommand(int num_cmds, const char ** cmds) {
  int i;
  const char * cmd;

  for (i = 1; i < num_cmds; i++) {
    cmd = cmds[i];
    if (cmd[0] == '-') continue;
    if (get_cmd(cmd))
      return i;
  }

  return -1;
}


int main(int argc, char * const * argv) {
  char c;
  int ok;
  int i;
  int subcommand_loc;
  const char * chosen_cmd = NULL;
  cmd_t * cmd = NULL;
  int no_more_args;

  extra_csv_opts_t opts = {
    .output_delim = -1
  };

  if (argc == 1) {
    usage(EXIT_FAILURE);
  }

  struct csv_parser parser;
  csv_init(&parser, 0);

  subcommand_loc = find_subcommand(argc, (const char **)argv);
  subcommand_loc = subcommand_loc > 0 ? subcommand_loc : argc;

  while ((c = getopt_long (subcommand_loc, argv, "q:d:o:th", longopts, NULL)) != -1) {
    switch (c) {
      case 'q': {
        csv_set_quote(&parser, optarg[0]);
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
        opts.output_delim = optarg[0];
        break;
      }
      case 't': {
        opts.output_delim = '\t';
        break;
      }
      case '?': {
        if (c == 'd') {
          // use , input delimination by default
          delim = ',';
        }
        break;
      }
      default: {
        printf("csv exiting because of %d\n", c);
        usage (EXIT_FAILURE);
      }
    }
  }

  if (opts.output_delim == -1) {
    opts.output_delim = delim;
  }

  // init parser settings
  csv_set_delim(&parser, delim);

  // choose command
  chosen_cmd = optind == argc ? "id" : argv[optind];

  // get the command
  cmd = get_cmd(chosen_cmd);

  if (!cmd) {
    ok = 0;
  }
  else {
    int new_argc = argc - subcommand_loc;
    ok = cmd->fn(&parser, &opts, new_argc, &argv[subcommand_loc]);
  }

  csv_free(&parser);

  exit (ok ? EXIT_SUCCESS : EXIT_FAILURE);
}
