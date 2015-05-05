
#ifndef CSV_SUBCOMMANDS_H
#define CSV_SUBCOMMANDS_H

typedef struct extra_csv_opts_t {
  char output_delim;
} extra_csv_opts_t;

struct csv_parser;
extern int cmd_cut(struct csv_parser * parser, extra_csv_opts_t *opts, int argc, const char **argv);
extern int cmd_grep(struct csv_parser * parser, extra_csv_opts_t *opts, int argc, const char **argv);

#endif /* CSV_SUBCOMMANDS_H */
