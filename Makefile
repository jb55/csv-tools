
BIN ?= csv-cut csv-tabulate
PREFIX ?= /usr/local
CFLAGS = 
#CFLAGS = -L${JEMALLOC_PATH}/lib -Wl,-rpath,${JEMALLOC_PATH}/lib -ljemalloc 
DEPS = $(wildcard deps/*/*.c)

all: csv-tabulate csv-cut

csv-cut: cut.c $(DEPS)
	$(CC) $(CFLAGS) -Ideps -o $@ $^

csv-tabulate: tabulate.c $(DEPS)
	$(CC) $(CFLAGS) -Ideps -o $@ $^

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	rm -f csv-cut
	rm -f csv-tabulate

