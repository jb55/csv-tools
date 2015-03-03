
# csv-tools

Fast csv tools in C. Work in progress.

## Examples

### csv

The base command, used for configuring input and output delimiters, quote
characters, etc.

### csv cut

`cut` analog for CSVs

Output tabs, cutting fields `2`, `3`, `4` to `6`, `11`, and everything after `11`:

    $ csv -t cut -f 2,3,4-6,11- < huge_data.csv

## License

  GPLv2
