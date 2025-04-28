#include "arquivo.h"
#include <sys/stat.h>
#include <unistd.h>

// Move um bloco de dados dentro do arquivo
int moverDados(FILE* vc, long offsetInicio, long offsetFim, size_t tamanho) {
    unsigned char buffer[8192];
    size_t bytesRestantes = tamanho;
    
    // Se movendo para frente, começa do fim
    if (offsetFim > offsetInicio) {
        long posAtual = offsetInicio + tamanho - 8192;
        long posFinal = offsetFim + tamanho - 8192;
        
        while (bytesRestantes > 0) {
            size_t bytesLer = (bytesRestantes < sizeof(buffer)) ? bytesRestantes : sizeof(buffer);
            
            fseek(vc, posAtual, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer) return -1;
            
            fseek(vc, posFinal, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer) return -1;
            
            posAtual -= sizeof(buffer);
            posFinal -= sizeof(buffer);
            bytesRestantes -= bytesLer;
        }
    } else {
        // Se movendo para trás, começa do início
        while (bytesRestantes > 0) {
            size_t bytesLer = (bytesRestantes < sizeof(buffer)) ? bytesRestantes : sizeof(buffer);
            
            fseek(vc, offsetInicio, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer) return -1;
            
            fseek(vc, offsetFim, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer) return -1;
            
            offsetInicio += bytesLer;
            offsetFim += bytesLer;
            bytesRestantes -= bytesLer;
        }
    }
    return 0;
}

// Reorganiza o arquivo removendo espaços vazios
int reorganizarArquivo(FILE* vc, ArquivoMembro* membros, int quantidade) {
    long offsetAtual = sizeof(uint32_t) + quantidade * sizeof(ArquivoMembro);
    
    for (int i = 0; i < quantidade; i++) {
        // Se o arquivo não está onde deveria
        if (membros[i].offset != offsetAtual) {
            if (moverDados(vc, membros[i].offset, offsetAtual, membros[i].emDisco) != 0) {
                return -1;
            }
            membros[i].offset = offsetAtual;
        }
        offsetAtual += membros[i].emDisco;
    }
    
    // Trunca o arquivo no novo tamanho
    ftruncate(fileno(vc), offsetAtual);
    return 0;
}

// Atualiza os offsets após uma movimentação
int atualizarOffsets(ArquivoMembro* membros, int quantidade, long offsetBase, long diferenca) {
    for (int i = 0; i < quantidade; i++) {
        if (membros[i].offset > offsetBase) {
            membros[i].offset += diferenca;
        }
    }
    return 0;
}

int inserirArquivo(const char* nomeArquivo, FILE* vc, long* offsetAtual,
                   ArquivoMembro* out, int compressao, int ordem) {
    FILE* arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) return -1;

    struct stat st;
    stat(nomeArquivo, &st);
    
    // Remove espaços do nome do arquivo
    char nomeTemp[256] = {0};
    int j = 0;
    for(int i = 0; nomeArquivo[i] && j < 255; i++) {
        if(nomeArquivo[i] != ' ') {
            nomeTemp[j++] = nomeArquivo[i];
        }
    }
    strncpy(out->nome, nomeTemp, 255);
    out->nome[255] = '\0';
    
    out->uid = getuid();
    out->tamanho = st.st_size;
    out->modificacao = st.st_mtime;
    out->ordem = ordem;
    out->offset = *offsetAtual;

    // Garante que estamos na posição correta para os dados
    fseek(vc, *offsetAtual, SEEK_SET);

    // Primeiro escreve os dados do arquivo
    if (compressao) {
        // Tenta comprimir o arquivo inteiro
        FILE* arquivo = fopen(nomeArquivo, "rb");
        if (!arquivo) return -1;

        // Lê todo o conteúdo do arquivo
        unsigned char* buffer = malloc(st.st_size);
        if (!buffer) {
            fclose(arquivo);
            return -1;
        }
        size_t bytesLidos = fread(buffer, 1, st.st_size, arquivo);
        
        // Tenta comprimir
        unsigned char* compBuf = malloc(st.st_size * 2); // Espaço extra para caso a compressão aumente
        if (!compBuf) {
            free(buffer);
            fclose(arquivo);
            return -1;
        }
        
        size_t compSize = LZ_Compress(buffer, compBuf, bytesLidos);
        
        // Se comprimido ficou maior, usa original
        if (compSize >= bytesLidos) {
            fwrite(buffer, 1, bytesLidos, vc);
            out->emDisco = bytesLidos;
        } else {
            fwrite(compBuf, 1, compSize, vc);
            out->emDisco = compSize;
        }

        free(buffer);
        free(compBuf);
        fclose(arquivo);
    } else {
        // Copia sem compressão
        FILE* arquivo = fopen(nomeArquivo, "rb");
        if (!arquivo) return -1;

        unsigned char buffer[8192];
        size_t bytesLidos;
        size_t totalBytes = 0;

        while ((bytesLidos = fread(buffer, 1, sizeof(buffer), arquivo)) > 0) {
            if (fwrite(buffer, 1, bytesLidos, vc) != bytesLidos) {
                fclose(arquivo);
                return -1;
            }
            totalBytes += bytesLidos;
        }
        out->emDisco = totalBytes;
        fclose(arquivo);
    }

    // Atualiza o offset para o próximo arquivo
    *offsetAtual += out->emDisco;
    
    return 0;
}

// Modifica a função extrairArquivo para usar buffers menores
int extrairArquivo(FILE* vc, ArquivoMembro membro, const char* pastaDestino) {
    char caminhoCompleto[512];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", pastaDestino, membro.nome);

    FILE* destino = fopen(caminhoCompleto, "wb");
    if (!destino) return -1;

    fseek(vc, membro.offset, SEEK_SET);

    if (membro.emDisco != membro.tamanho) {
        // Arquivo está comprimido
        unsigned char* compBuf = malloc(membro.emDisco);
        unsigned char* decompBuf = malloc(membro.tamanho);
        
        if (!compBuf || !decompBuf) {
            free(compBuf);
            free(decompBuf);
            fclose(destino);
            return -1;
        }

        // Lê o arquivo comprimido completo
        if (fread(compBuf, 1, membro.emDisco, vc) != membro.emDisco) {
            free(compBuf);
            free(decompBuf);
            fclose(destino);
            return -1;
        }

        // Descomprime e escreve
        LZ_Uncompress(compBuf, decompBuf, membro.emDisco);
        if (fwrite(decompBuf, 1, membro.tamanho, destino) != membro.tamanho) {
            free(compBuf);
            free(decompBuf);
            fclose(destino);
            return -1;
        }

        free(compBuf);
        free(decompBuf);
    } else { // Arquivo não está comprimido
        unsigned char buffer[8192];
        size_t bytesRestantes = membro.emDisco;
        
        while (bytesRestantes > 0) {
            size_t bytesLer = (bytesRestantes < sizeof(buffer)) ? bytesRestantes : sizeof(buffer);
            size_t bytesLidos = fread(buffer, 1, bytesLer, vc);
            fwrite(buffer, 1, bytesLidos, destino);
            bytesRestantes -= bytesLidos;
        }
    }

    fclose(destino);
    return 0;
}

int removerArquivo(ArquivoMembro* lista, int* qtd, const char* nome) {
    for (int i = 0; i < *qtd; i++) {
        if (strcmp(lista[i].nome, nome) == 0) {
            for (int j = i; j < *qtd - 1; j++) {
                lista[j] = lista[j + 1];
            }
            (*qtd)--;
            return 1;
        }
    }
    return 0;
}