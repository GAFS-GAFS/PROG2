# Nome do programa final (alterar conforme necessário)
nomePrograma = trab

# Parâmetros de compilação para exibir warnings e erros comuns
parametrosCompilacao = -Wall -Wextra -Wpedantic -Wconversion -Werror -g

# Lista de arquivos fonte (adicione/remova conforme necessário)
SRCS = main.c ordenacao.c

# Lista de arquivos objeto gerados a partir dos fontes
OBJS = $(SRCS:.c=.o)

# Regra principal: gera o executável
all: $(nomePrograma)

# Compilar o executável a partir dos arquivos objeto
$(nomePrograma): $(OBJS)
	gcc -o $(nomePrograma) $(OBJS) $(parametrosCompilacao)

# Regras para compilar arquivos .c em .o
main.o: main.c
	gcc -c main.c $(parametrosCompilacao)

ordenacao.o: ordenacao.h ordenacao.c
	gcc -c ordenacao.c $(parametrosCompilacao)

# Comando para limpar arquivos gerados (objetos e executável)
clean:
	rm -f *.o *.gch $(nomePrograma)

# Comando para exibir quais arquivos estão sendo usados
show:
	@echo "Executável gerado: ./$(nomePrograma)"
	@echo "Arquivos fonte: $(SRCS)"
	@echo "Arquivos objeto: $(OBJS)"

.PHONY: all clean show

#===================== MAKE FILE BASE PARA REALIZAR TESTES NOS EXERCICIOS =====================================#