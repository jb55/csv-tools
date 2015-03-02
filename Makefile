
BIN ?= csv
PREFIX ?= /usr/local
CFLAGS =
DEPS = $(wildcard deps/csv.c/*.c)
SRCS = $(wildcard src/*.c)

all: csv

$(BIN): $(SRCS) $(DEPS)
	$(CC) $^ $(CFLAGS) -Ideps -o $@

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	rm -f csv
