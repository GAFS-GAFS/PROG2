
#include "arquivo.h"

int inserirArquivo(const char* nomeArquivo, FILE* vc, long* offsetAtual,
    ArquivoMembro* out, int compressao, int ordem) {
printf("Stub: inserirArquivo chamado para '%s'.\n", nomeArquivo);
return 0;
}

int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino) {
printf("Stub: extrairArquivo chamado.\n");
return 0;
}

int removerArquivo(ArquivoMembro* lista, int* qtd, const char* nome) {
printf("Stub: removerArquivo chamado para '%s'.\n", nome);
return 1;
}