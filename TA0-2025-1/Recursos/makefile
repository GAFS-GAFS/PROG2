parametrosCompilacao=-Wall #-Wshadow
nomePrograma=trab

all: $(nomePrograma)

$(nomePrograma): main.o jukefila.o pedido.o
	gcc -o $(nomePrograma) main.o jukefila.o pedido.o $(parametrosCompilacao)

main.o: main.c
	gcc -c main.c $(parametrosCompilacao)

jukefila.o: jukefila.h jukefila.c
	gcc -c jukefila.c $(parametrosCompilacao)

pedido.o: pedido.h pedido.c
	gcc -c pedido.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch $(nomePrograma)

