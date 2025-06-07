TARGET = jogo

SRC = main.c menu.c controls.c background.c character.c bullet.c pistol.c joystick.c
OBJ = $(SRC:.c=.o)

CC = gcc

CFLAGS = -Wall -std=c11 `pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5`
LDFLAGS = `pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5`

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
