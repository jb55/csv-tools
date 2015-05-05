
BIN ?= csv
PREFIX ?= /usr/local
CFLAGS = -Weverything

OBJS  = deps/csv.c/csv.o
OBJS += deps/field-range-parser/field-range-parser.o

OBJS += src/csv.o
OBJS += src/inference.o
OBJS += src/cut.o
OBJS += src/util.o
OBJS += src/grep.o

all: csv

%.o: %.c
	$(CC) -Ideps -o $@ -c $<

$(BIN): $(OBJS)
	$(CC) $^ $(CFLAGS) -Ideps -o $@

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	find . -name '*.o' -exec rm -f {} \;
	 rm -f csv
