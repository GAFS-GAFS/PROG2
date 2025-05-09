#include "arquivo.h"

// Função auxiliar para encontrar o maior arquivo no archive
size_t encontrarMaiorArquivo(Lista* lista, size_t tamanhoAtual) {
    size_t maiorTamanho = tamanhoAtual;
    
    No* atual = lista->primeiro;
    while (atual != NULL) {
        if (atual->arquivo.tamanho > maiorTamanho) {
            maiorTamanho = atual->arquivo.tamanho;
        }
        atual = atual->proximo;
    }
    
    return maiorTamanho;
}

// Move um bloco de dados dentro do arquivo
int moverDados(FILE* vc, uint64_t offsetInicio, uint64_t offsetFim, size_t tamanho) {
    // Encontra tamanho do maior arquivo para alocar buffer
    rewind(vc);
    uint32_t numMembros;
    fread(&numMembros, sizeof(uint32_t), 1, vc);
    
    // Aloca buffer para ler os metadados
    Lista lista;
    inicializarLista(&lista);
    
    // Lê os metadados dos arquivos
    for (uint32_t i = 0; i < numMembros; i++) {
        ArquivoMembro membro;
        fread(&membro, sizeof(ArquivoMembro), 1, vc);
        inserirArquivoLista(&lista, membro);
    }
    
    size_t tamanhoBuffer = encontrarMaiorArquivo(&lista, tamanho);
    unsigned char* buffer = malloc(tamanhoBuffer);
    
    if (!buffer) {
        destruirLista(&lista);
        return -1;
    }

    // Se movendo para frente, começa do fim
    if (offsetFim > offsetInicio) {
        size_t bytesRestantes = tamanho;
        while (bytesRestantes > 0) {
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;
            
            fseek(vc, offsetInicio + bytesRestantes - bytesLer, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer) {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }
            
            fseek(vc, offsetFim + bytesRestantes - bytesLer, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer) {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }
            
            bytesRestantes -= bytesLer;
        }
    } else {
        // Se movendo para trás, começa do início
        size_t bytesRestantes = tamanho;
        while (bytesRestantes > 0) {
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;
            
            fseek(vc, offsetInicio, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer) {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }
            
            fseek(vc, offsetFim, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer) {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }
            
            offsetInicio += bytesLer;
            offsetFim += bytesLer;
            bytesRestantes -= bytesLer;
        }
    }
    
    free(buffer);
    destruirLista(&lista);
    return 0;
}

// Função de inserção ajustada para evitar gaps
int inserirArquivo(const char* nomeArquivo, FILE* vc, uint64_t* offsetAtual, uint32_t* tamanhoFinal, uint32_t compressao) {
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return -1;

    // Obtém o tamanho do arquivo
    fseek(arquivo, 0, SEEK_END);
    size_t tamanhoOriginal = ftell(arquivo);
    rewind(arquivo);

    // Lê o arquivo fonte
    unsigned char* buffer = malloc(tamanhoOriginal);
    if (!buffer) {
        fclose(arquivo);
        return -1;
    }

    // Lê o arquivo fonte para o buffer
    if (fread(buffer, 1, tamanhoOriginal, arquivo) != tamanhoOriginal) {
        free(buffer);
        fclose(arquivo);
        return -1;
    }
    fclose(arquivo);

    // Prepara os dados finais (comprimidos ou não)
    unsigned char* dadosFinais = buffer;
    *tamanhoFinal = (uint32_t)tamanhoOriginal;
    unsigned char* bufferComprimido = NULL;

    if (compressao) {
        // Tenta comprimir os dados
        bufferComprimido = malloc(tamanhoOriginal);
        if (!bufferComprimido) {
            free(buffer);
            return -1;
        }

        int tamanhoComprimido = LZ_Compress(buffer, bufferComprimido, (unsigned int)tamanhoOriginal);
        if (tamanhoComprimido > 0 && (uint32_t)tamanhoComprimido < (uint32_t)tamanhoOriginal) {
            dadosFinais = bufferComprimido;
            *tamanhoFinal = (uint32_t)tamanhoComprimido;
        }
    }

    // Posiciona no offset atual correto e escreve os dados
    fseek(vc, *offsetAtual, SEEK_SET);
    size_t bytesEscritos = fwrite(dadosFinais, 1, *tamanhoFinal, vc);

    // Limpa a memória
    free(buffer);
    if (bufferComprimido) free(bufferComprimido);

    // Atualiza o offset para o próximo arquivo
    *offsetAtual += *tamanhoFinal;

    // Retorna sucesso apenas se escreveu todos os bytes
    return (bytesEscritos == *tamanhoFinal) ? 0 : -1;
}


int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino) {
    char caminhoCompleto[2048];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", pastaDestino, membro.nome);

    FILE* destino = fopen(caminhoCompleto, "wb");
    if (!destino) return -1;

    // Encontra tamanho do maior arquivo para buffer
    rewind(vc);
    uint32_t numMembros;
    fread(&numMembros, sizeof(uint32_t), 1, vc);
    
    Lista lista;
    inicializarLista(&lista);
    
    for (uint32_t i = 0; i < numMembros; i++) {
        ArquivoMembro membroTemp;
        fread(&membroTemp, sizeof(ArquivoMembro), 1, vc);
        inserirArquivoLista(&lista, membroTemp);
    }
    
    size_t tamanhoBuffer = encontrarMaiorArquivo(&lista, membro.tamanho);
    unsigned char* buffer = malloc(tamanhoBuffer);

    if (!buffer) {
        destruirLista(&lista);
        fclose(destino);
        return -1;
    }

    fseek(vc, membro.offset, SEEK_SET);

    if (membro.emDisco != membro.tamanho) {
        // Arquivo está comprimido
        if (fread(buffer, 1, membro.emDisco, vc) != membro.emDisco) {
            free(buffer);
            destruirLista(&lista);
            fclose(destino);
            return -1;
        }

        // Descomprime
        unsigned char* decompBuf = malloc(membro.tamanho);
        if (!decompBuf) {
            free(buffer);
            destruirLista(&lista);
            fclose(destino);
            return -1;
        }

        LZ_Uncompress(buffer, decompBuf, membro.emDisco);
        fwrite(decompBuf, 1, membro.tamanho, destino);
        free(decompBuf);
    } else {
        // Arquivo não comprimido
        if (fread(buffer, 1, membro.tamanho, vc) != membro.tamanho) {
            free(buffer);
            destruirLista(&lista);
            fclose(destino);
            return -1;
        }
        fwrite(buffer, 1, membro.tamanho, destino);
    }

    free(buffer);
    destruirLista(&lista);
    fclose(destino);
    return 0;
}

void ajustarMetadados(FILE* vc, Lista* lista, uint32_t posRemovida) {
    if (!lista || !lista->primeiro) return;
    
    // Encontra o nó a ser removido
    No* noRemover = lista->primeiro;
    for (uint32_t i = 0; i < posRemovida && noRemover; i++) {
        noRemover = noRemover->proximo;
    }
    
    if (!noRemover) return;
    
    uint64_t offsetRemovido = noRemover->arquivo.offset;
    
    // Move os arquivos que estão após o removido para frente no disco
    No* atual = noRemover->proximo;
    while (atual != NULL) {
        if (atual->arquivo.offset > offsetRemovido) {
            // Move o arquivo fisicamente
            moverDados(vc, atual->arquivo.offset, offsetRemovido, atual->arquivo.emDisco);
            // Atualiza o offset do arquivo movido
            atual->arquivo.offset = offsetRemovido;
            offsetRemovido += atual->arquivo.emDisco;
        }
        atual = atual->proximo;
    }

    // Remove o nó da lista
    if (noRemover->anterior) {
        noRemover->anterior->proximo = noRemover->proximo;
    } else {
        lista->primeiro = noRemover->proximo;
    }
    
    if (noRemover->proximo) {
        noRemover->proximo->anterior = noRemover->anterior;
    } else {
        lista->ultimo = noRemover->anterior;
    }
    
    // Atualiza UIDs e ordens
    atual = noRemover->proximo;
    while (atual != NULL) {
        if (atual->arquivo.uid > noRemover->arquivo.uid) {
            atual->arquivo.uid--;
        }
        atual = atual->proximo;
    }
    
    free(noRemover);
    lista->quantidade--;
    atualizarOrdens(lista);
}

int removerArquivo(FILE* vc, Lista* lista, const char* nome) {
    No* atual = lista->primeiro;
    uint32_t posicao = 0;
    
    while (atual != NULL) {
        if (strcmp(atual->arquivo.nome, nome) == 0) {
            ajustarMetadados(vc, lista, posicao);
            
            // Trunca o arquivo no novo tamanho após a remoção
            uint64_t novoTamanho = sizeof(uint32_t) + (lista->quantidade) * sizeof(ArquivoMembro);
            
            atual = lista->primeiro;
            while (atual != NULL) {
                if (atual->arquivo.offset + atual->arquivo.emDisco > novoTamanho) {
                    novoTamanho = atual->arquivo.offset + atual->arquivo.emDisco;
                }
                atual = atual->proximo;
            }
            
            ftruncate(fileno(vc), novoTamanho);
            return 1;
        }
        atual = atual->proximo;
        posicao++;
    }
    return 0;
}