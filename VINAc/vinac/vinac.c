#include "vinac.h"

// Insere arquivos no arquivador .vc.
void vinac_insert(int argc, char *argv[], int compress) {
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -ip|-ic arquivo.vc arquivos...\n");
        return;
    }

    const char* arquivoVC = argv[2];

    // Se o arquivo não for especificado, usa o nome padrão "archive.vc"
    if (strcmp(arquivoVC, "") == 0) {
        arquivoVC = "archive.vc";
        printf("Nenhum arquivo especificado. Usando o nome padrão '%s'.\n", arquivoVC);
    }

    FILE* vc = fopen(arquivoVC, "r+b");

    // Se o arquivo não existir, cria um novo
    if (!vc) {
        vc = fopen(arquivoVC, "w+b");
        if (!vc) {
            fprintf(stderr, "Erro ao criar/abrir %s\n", arquivoVC);
            return;
        }
        printf("Arquivo '%s' criado com sucesso.\n", arquivoVC);
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice existente, se houver
    carregarIndice(vc, &indice);

    // Move o ponteiro para o final do arquivo
    fseek(vc, 0, SEEK_END);
    long offsetAtual = ftell(vc);

    // Insere cada arquivo especificado
    for (int i = 3; i < argc; i++) {
        ArquivoMembro membro;
        if (inserirArquivo(argv[i], vc, &offsetAtual, &membro, compress, indice.quantidade) == 0) {
            adicionarAoIndice(&indice, membro);
            printf("Arquivo '%s' inserido com sucesso.\n", argv[i]);
        } else {
            fprintf(stderr, "Erro ao inserir o arquivo '%s'.\n", argv[i]);
        }
    }

    // Salva o índice atualizado
    salvarIndice(vc, &indice);

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Remove um ou mais arquivos do arquivador .vc.
void vinac_remove(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -r arquivo.vc arquivos...\n");
        return;
    }

    const char* arquivoVC = argv[2];

    // Se o arquivo não for especificado, usa o nome padrão "archive.vc"
    if (strcmp(arquivoVC, "") == 0) {
        arquivoVC = "archive.vc";
        printf("Nenhum arquivo especificado. Usando o nome padrão '%s'.\n", arquivoVC);
    }

    FILE* vc = fopen(arquivoVC, "r+b");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Remove os arquivos especificados
    for (int i = 3; i < argc; i++) {
        if (removerArquivo(indice.membros, &indice.quantidade, argv[i])) {
            printf("Arquivo '%s' removido com sucesso.\n", argv[i]);
        } else {
            fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", argv[i]);
        }
    }

    // Salva o índice atualizado
    salvarIndice(vc, &indice);

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Lista todos os arquivos presentes no arquivador .vc.
void vinac_list(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: vinac -c arquivo.vc\n");
        return;
    }

    const char* arquivoVC = argv[2];

    // Se o arquivo não for especificado, usa o nome padrão "archive.vc"
    if (strcmp(arquivoVC, "") == 0) {
        arquivoVC = "archive.vc";
        printf("Nenhum arquivo especificado. Usando o nome padrão '%s'.\n", arquivoVC);
    }

    FILE* vc = fopen(arquivoVC, "rb");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Lista os arquivos
    listarArquivos(&indice);

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Extrai um ou mais arquivos do arquivador .vc para o sistema de arquivos.
void vinac_extract(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: vinac -x arquivo.vc arquivos...\n");
        return;
    }

    const char* arquivoVC = argv[2];

    // Se o arquivo não for especificado, usa o nome padrão "archive.vc"
    if (strcmp(arquivoVC, "") == 0) {
        arquivoVC = "archive.vc";
        printf("Nenhum arquivo especificado. Usando o nome padrão '%s'.\n", arquivoVC);
    }

    FILE* vc = fopen(arquivoVC, "rb");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice
    if (carregarIndice(vc, &indice) < 0) {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Extrai os arquivos especificados
    for (int i = 3; i < argc; i++) {
        const char* nomeExtrair = argv[i];
        int encontrado = 0;

        for (int j = 0; j < indice.quantidade; j++) {
            if (strcmp(indice.membros[j].nome, nomeExtrair) == 0) {
                if (extrairArquivo(vc, indice.membros[j], ".") == 0) {
                    printf("Arquivo '%s' extraído com sucesso.\n", nomeExtrair);
                } else {
                    fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", nomeExtrair);
                }
                encontrado = 1;
                break;
            }
        }

        if (!encontrado) {
            fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", nomeExtrair);
        }
    }

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}

// Move a posição lógica de um arquivo dentro do arquivador.
void vinac_move(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso: vinac -m arquivo.vc posicao_atual nova_posicao\n");
        return;
    }

    const char* arquivoVC = argv[2];

    // Se o arquivo não for especificado, usa o nome padrão "archive.vc"
    if (strcmp(arquivoVC, "") == 0) {
        arquivoVC = "archive.vc";
        printf("Nenhum arquivo especificado. Usando o nome padrão '%s'.\n", arquivoVC);
    }

    int posicaoAtual = atoi(argv[3]);
    int novaPosicao = atoi(argv[4]);

    FILE* vc = fopen(arquivoVC, "r+b");
    if (!vc) {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    // Inicializa o índice
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice
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
        for (int i = posicaoAtual; i < novaPosicao; i++) {
            indice.membros[i] = indice.membros[i + 1];
        }
    } else if (posicaoAtual > novaPosicao) {
        for (int i = posicaoAtual; i > novaPosicao; i--) {
            indice.membros[i] = indice.membros[i - 1];
        }
    }
    indice.membros[novaPosicao] = temp;

    printf("Arquivo movido da posição %d para %d com sucesso.\n", posicaoAtual, novaPosicao);

    // Salva o índice atualizado
    salvarIndice(vc, &indice);

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);
}