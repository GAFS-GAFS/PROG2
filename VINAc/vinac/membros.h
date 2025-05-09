#ifndef MEMBROS_H
#define MEMBROS_H

#include <stdint.h>
#include <time.h>

// Define a estrutura básica que representa um arquivo dentro do arquivador .vc
// Esta estrutura mantém todos os metadados necessários para gerenciar cada arquivo
typedef struct {
    char nome[1024];        // Nome do arquivo (limitado a 1024 caracteres)
    uint32_t uid;          // Identificador único do arquivo
    uint32_t tamanho;      // Tamanho original do arquivo em bytes
    uint32_t emDisco;      // Tamanho do arquivo após compressão (se houver)
    time_t modificacao;    // Data da última modificação do arquivo
    uint32_t ordem;        // Ordem do arquivo na lista
    uint64_t offset;       // Posição do arquivo dentro do .vc (64 bits para arquivos grandes)
} ArquivoMembro;

#endif