CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99
LDFLAGS =
SOURCES = src/graph.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = build/graph

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@mkdir -p build
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

clean:
	rm -rf build/*

.PHONY: all clean
