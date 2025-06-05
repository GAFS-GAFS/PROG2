# Nome do executável
TARGET = jogo

# Fontes do projeto
SRC = main.c menu.c controls.c

# Compilador
CC = gcc

# Flags do compilador (inclui Allegro)
CFLAGS = -Wall `pkg-config --cflags allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5`

# Bibliotecas Allegro
LDFLAGS = `pkg-config --libs allegro-5 allegro_font-5 allegro_ttf-5 allegro_image-5 allegro_primitives-5`

# Regra padrão
all: $(TARGET)

# Compilação
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Limpeza
clean:
	rm -f $(TARGET)
