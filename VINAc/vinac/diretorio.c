#include "diretorio.h"


void inicializarIndice(IndiceArquivador* idx) {
    idx->membros = NULL;
    idx->quantidade = 0;
    idx->capacidade = 0;
}

void destruirIndice(IndiceArquivador* idx) {
    free(idx->membros);
    idx->membros = NULL;
    idx->quantidade = 0;
    idx->capacidade = 0;
}

void adicionarAoIndice(IndiceArquivador* idx, ArquivoMembro membro) {
    // Verifica se arquivo já existe
    for (int i = 0; i < idx->quantidade; i++) {
        if (strcmp(idx->membros[i].nome, membro.nome) == 0) {
            // Mantém a ordem original se já existe
            membro.ordem = idx->membros[i].ordem;
            idx->membros[i] = membro;
            return;
        }
    }

    // Novo arquivo - verifica se precisa aumentar o array
    if (idx->quantidade >= idx->capacidade) {
        int novaCapacidade = (idx->capacidade == 0) ? 1 : idx->capacidade * 2;
        ArquivoMembro* novoArray = realloc(idx->membros, novaCapacidade * sizeof(ArquivoMembro));
        if (!novoArray) return;
        idx->membros = novoArray;
        idx->capacidade = novaCapacidade;
    }
    
    idx->membros[idx->quantidade++] = membro;
}

int carregarIndice(FILE* vc, IndiceArquivador* idx) {
    rewind(vc);
    
    // Lê quantidade de membros
    uint32_t numMembros;
    if (fread(&numMembros, sizeof(uint32_t), 1, vc) != 1) {
        return -1;
    }
    
    // Aloca espaço
    idx->membros = malloc(numMembros * sizeof(ArquivoMembro));
    if (!idx->membros) return -1;
    
    // Lê cada membro do diretório
    for (uint32_t i = 0; i < numMembros; i++) {
        ArquivoMembro* membro = &idx->membros[i];
        if (fread(membro, sizeof(ArquivoMembro), 1, vc) != 1) {
            free(idx->membros);
            return -1;
        }
    }
    
    idx->quantidade = numMembros;
    idx->capacidade = numMembros;
    return 0;
}

int salvarIndice(FILE* vc, IndiceArquivador* idx) {
    // Calcula tamanho total dos dados dos membros
    long posicaoAtual = ftell(vc);
    
    // Move para o início do arquivo
    fseek(vc, 0, SEEK_SET);
    
    // Escreve cabeçalho com quantidade de membros
    uint32_t numMembros = idx->quantidade;
    if (fwrite(&numMembros, sizeof(uint32_t), 1, vc) != 1) {
        return -1;
    }
    
    // Escreve cada membro na ordem correta
    for (int i = 0; i < idx->quantidade; i++) {
        ArquivoMembro* atual = NULL;
        // Encontra membro com ordem i
        for (int j = 0; j < idx->quantidade; j++) {
            if (idx->membros[j].ordem == i) {
                atual = &idx->membros[j];
                break;
            }
        }
        if (!atual || fwrite(atual, sizeof(ArquivoMembro), 1, vc) != 1) {
            return -1;
        }
    }
    
    // Retorna à posição original
    fseek(vc, posicaoAtual, SEEK_SET);
    return 0;
}

void listarArquivos(const IndiceArquivador* idx) {
    printf("Conteúdo do arquivo:\n");
    printf("Ordem  Nome                Uid      Tam.Orig  Tam.Disco  Data Mod.\n");
    printf("-----  ------------------- -------- --------- ---------- ----------\n");
    
    for (int i = 0; i < idx->quantidade; i++) {
        ArquivoMembro* m = &idx->membros[i];
        char data[20];
        strftime(data, 20, "%Y-%m-%d", localtime(&m->modificacao));
        printf("%-5d  %-19s %-8u %-9u %-10u %s\n",
               m->ordem,
               m->nome,
               m->uid,
               m->tamanho,
               m->emDisco,
               data);
    }
}
