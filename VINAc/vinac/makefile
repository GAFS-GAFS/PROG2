# Configurações do compilador e flags
CC=gcc                    # Compilador a ser usado
CFLAGS=-Wall -Wextra -g  # Flags de compilação: todos os warnings e informações de debug
LDFLAGS=                 # Flags de linkedição (vazio por enquanto)

# Arquivos fonte do projeto
SOURCES=main.c vinac.c diretorio.c arquivo.c lz.c lista.c
OBJECTS=$(SOURCES:.c=.o) # Gera lista de objetos a partir dos fontes
TARGET=vina              # Nome do executável final

# Regra principal - compila todo o projeto
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Regra genérica para compilar arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regra para limpar os arquivos gerados
clean:
	rm -f $(OBJECTS) $(TARGET)

# Indica que 'all' e 'clean' não são arquivos
.PHONY: all clean
