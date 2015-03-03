
BIN ?= csv
PREFIX ?= /usr/local
CFLAGS =
DEPS  = $(wildcard deps/csv.c/*.c)
DEPS += $(wildcard deps/field-range-parser/field-range-parser.c)
SRCS = $(wildcard src/*.c)

all: csv

$(BIN): $(SRCS) $(DEPS)
	$(CC) $^ $(CFLAGS) -Ideps -o $@

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	rm -f csv
