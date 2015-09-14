# csv-tools

Deprecated

Check out [csv-delim](https://github.com/jb55/csv-delim) for a simplified
tool that adheres to the Unix philosphy a bit better.

## Description

Fast csv tools in C. Work in progress.

## Examples

### csv

The base command, used for configuring input and output delimiters, quote
characters, etc.

### csv cut

`cut` analog for CSVs

Output tabs, cutting fields `2`, `3`, `4` to `6`, `11`, and everything after `11`:

    $ csv -t cut -f 2,3,4-6,11- < huge_data.csv

### csv grep

`grep` targetting specific fields

Note, this is really simply at the moment. It supports exact matching
from the start of the field. This will be replaced with regex in the near
future

    $ csv grep -f3,2 'William' < huge_data.csv

## License

  GPLv2
