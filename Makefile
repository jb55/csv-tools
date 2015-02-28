
BIN ?= csv-cut csv-tabulate
PREFIX ?= /usr/local
CFLAGS =
#CFLAGS = -L${JEMALLOC_PATH}/lib -Wl,-rpath,${JEMALLOC_PATH}/lib -ljemalloc
DEPS = $(wildcard deps/*/*.c)

DST = bin

all: $(DST)/csv-tabulate $(DST)/csv-cut

$(DST)/csv-cut: src/cut.c $(DEPS)
	$(CC) $(CFLAGS) -Ideps -o $@ $^

$(DST)/csv-tabulate: src/tabulate.c $(DEPS)
	$(CC) $(CFLAGS) -Ideps -o $@ $^

test: $(DST)/csv-tabulate
	./test/run

install: $(BIN)
	install $^ $(PREFIX)/bin

clean:
	rm -f $(DST)/csv-cut
	rm -f $(DST)/csv-tabulate
