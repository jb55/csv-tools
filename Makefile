
BIN ?= csv
PREFIX ?= /usr/local
CFLAGS =
DEPS = $(wildcard deps/*/*.c)
SRCS = $(wildcard src/*.c)

all: csv

$(BIN): $(SRCS) $(DEPS)
	$(CC) $(CFLAGS) -Ideps -o $@ $^

test: $(DST)/csv-tabulate
	./test/run

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	rm -f csv
