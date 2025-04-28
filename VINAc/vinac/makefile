CC=gcc
CFLAGS=-Wall -Wextra -g
LDFLAGS=
SOURCES=main.c vinac.c diretorio.c arquivo.c lz.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=vinac

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
