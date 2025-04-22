#include "vinac.h"

#include <stdio.h>
#include <string.h>
#include "diretorio.h"
#include "arquivo.h"

// Insere arquivos no arquivador .vc.
// argc       Quantidade de argumentos da linha de comando.
// argv       Vetor de argumentos da linha de comando.
// compress   Define se os arquivos devem ser comprimidos (1) ou não (0).
void vinac_insert(int argc, char *argv[], int compress) {
    // Verifica se o número de argumentos é suficiente
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -ip|-ic arquivo.vc arquivos...\n");
        return;
    }

    // Nome do arquivo .vc a ser criado ou utilizado
    const char* arquivoVC = argv[2];

    // Tenta abrir o arquivo .vc existente para leitura e escrita
    FILE* vc = fopen(arquivoVC, "r+b");

    // Se não existir, cria um novo
    if (!vc) {
        vc = fopen(arquivoVC, "w+b");
        if (!vc) {
            fprintf(stderr, "Erro ao criar/abrir %s\n", arquivoVC);
            return;
        }
    }

    // Inicializa a estrutura de índice em memória (RAM)
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Tenta carregar índice já existente do .vc (caso o arquivo já tenha membros)
    carregarIndice(vc, &indice);

    // Inicializa o offset atual no final do arquivo .vc
    fseek(vc, 0, SEEK_END);
    long offsetAtual = ftell(vc);

    // Loop pelos arquivos passados na linha de comando
    for (int i = 3; i < argc; i++) {
        ArquivoMembro membro;

        // Insere o arquivo no .vc (com ou sem compressão)
        if (inserirArquivo(argv[i], vc, &offsetAtual, &membro, compress) == 0) {
            // Se sucesso, adiciona o membro ao índice
            adicionarAoIndice(&indice, membro);
        } else {
            // Em caso de erro, exibe mensagem mas continua com os demais
            fprintf(stderr, "Erro ao inserir arquivo: %s\n", argv[i]);
        }
    }

    // Salva o índice atualizado no .vc (final do arquivo)
    salvarIndice(vc, &indice);

    // Libera memória alocada para o índice
    destruirIndice(&indice);

    // Fecha o arquivo .vc
    fclose(vc);
}

// Remove um ou mais arquivos do arquivador .vc.
// argc: Quantidade de argumentos da linha de comando.
// argv: Vetor de argumentos da linha de comando.
void vinac_remove(int argc, char *argv[]) {
    // Verifica se há argumentos suficientes
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -rm arquivo.vc arquivos...\n");
        return;
    }

    // Nome do arquivo .vc (sempre o terceiro argumento)
    const char* nomeArquivoVC = argv[2];

    // Abre o arquivo .vc para leitura e escrita binária
    FILE* vc = fopen(nomeArquivoVC, "r+b");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivoVC);
        return;
    }

    // Inicializa o índice de arquivos
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega os metadados do .vc para RAM
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Processa cada nome de arquivo a ser removido
    for (int i = 3; i < argc; i++) {
        const char* nomeRemover = argv[i];
        int removido = removerArquivo(indice.membros, &indice.quantidade, nomeRemover);

        if (removido) {
            printf("Arquivo '%s' removido do índice.\n", nomeRemover);
        } else {
            fprintf(stderr, "Arquivo '%s' não encontrado no índice.\n", nomeRemover);
        }
    }

    // Salva o índice atualizado de volta no arquivo .vc
    if (salvarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao salvar o índice atualizado no arquivador.\n");
    }

    // Libera a memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Lista todos os arquivos presentes no arquivador .vc.
// argc: Quantidade de argumentos da linha de comando.
// argv: Vetor de argumentos da linha de comando.
void vinac_list(int argc, char *argv[]) {
    // Verifica se foi fornecido o nome do .vc
    if (argc < 3) {
        fprintf(stderr, "Uso: vinac -l arquivo.vc\n");
        return;
    }

    // Nome do arquivo .vc
    const char* nomeArquivoVC = argv[2];

    // Abre o arquivo .vc em modo leitura
    FILE* vc = fopen(nomeArquivoVC, "rb");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice do arquivador
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Lista os arquivos com metadados
    listarArquivos(&indice);

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}


// Extrai um ou mais arquivos do arquivador .vc para o disco.
// argc: Quantidade de argumentos da linha de comando.
// argv: Vetor de argumentos da linha de comando.
void vinac_extract(int argc, char *argv[]) {
    // Verifica se há argumentos suficientes
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -x arquivo.vc arquivos...\n");
        return;
    }

    // Nome do arquivo .vc
    const char* nomeArquivoVC = argv[2];

    // Abre o arquivo .vc para leitura binária
    FILE* vc = fopen(nomeArquivoVC, "rb");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice do arquivo .vc para RAM
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Itera sobre os nomes dos arquivos a serem extraídos
    for (int i = 3; i < argc; i++) {
        const char* nomeExtrair = argv[i];
        int encontrado = 0;

        // Procura o arquivo pelo nome no índice
        for (int j = 0; j < indice.quantidade; j++) {
            if (strcmp(indice.membros[j].nome, nomeExtrair) == 0) {
                // Arquivo encontrado, tenta extrair
                if (extrairArquivo(vc, indice.membros[j], "." /* pasta atual */) == 0) {
                    printf("Arquivo '%s' extraído com sucesso.\n", nomeExtrair);
                } else {
                    fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", nomeExtrair);
                }
                encontrado = 1;
                break;
            }
        }

        // Se não encontrou o arquivo no índice
        if (!encontrado) {
            fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", nomeExtrair);
        }
    }

    // Libera a memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Move a posição lógica de um arquivo dentro do índice do arquivador .vc.
// argc: Quantidade de argumentos da linha de comando.
// argv: Vetor de argumentos da linha de comando.
void vinac_move(int argc, char *argv[]) {
    // Verifica se há argumentos suficientes
    if (argc < 5) {
        fprintf(stderr, "Uso: vinac -mv arquivo.vc posicao_atual nova_posicao\n");
        return;
    }

    // Nome do arquivo .vc
    const char* nomeArquivoVC = argv[2];

    // Posições fornecidas pelo usuário
    int posicaoAtual = atoi(argv[3]);
    int novaPosicao = atoi(argv[4]);

    // Abre o arquivo .vc para leitura e escrita binária
    FILE* vc = fopen(nomeArquivoVC, "r+b");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", nomeArquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice do arquivador
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Verifica se as posições são válidas
    if (posicaoAtual < 0 || posicaoAtual >= indice.quantidade ||
        novaPosicao < 0 || novaPosicao >= indice.quantidade) {
        fprintf(stderr, "Posições inválidas. O índice deve estar entre 0 e %d.\n", indice.quantidade - 1);
        destruirIndice(&indice);
        fclose(vc);
        return;
    }

    // Realiza a troca lógica no índice
    ArquivoMembro temp = indice.membros[posicaoAtual];
    if (posicaoAtual < novaPosicao) {
        // Move os elementos para a esquerda
        for (int i = posicaoAtual; i < novaPosicao; i++) {
            indice.membros[i] = indice.membros[i + 1];
        }
    } else if (posicaoAtual > novaPosicao) {
        // Move os elementos para a direita
        for (int i = posicaoAtual; i > novaPosicao; i--) {
            indice.membros[i] = indice.membros[i - 1];
        }
    }
    indice.membros[novaPosicao] = temp;

    printf("Arquivo movido da posição %d para %d com sucesso.\n", posicaoAtual, novaPosicao);

    // Salva o índice atualizado no arquivador
    if (salvarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao salvar o índice atualizado no arquivador.\n");
    }

    // Libera a memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}