#include "vinac.h"

// Insere um ou mais arquivos no arquivador .vc
// Se o arquivo já existir, é atualizado mantendo sua ordem mas com novo UID
void vinac_insert(int argc, char *argv[], uint32_t compress)
{
    if (argc < 4)
    {
        fprintf(stderr, "Uso: vinac -i[c] arquivo.vc arquivo1 [arquivo2 ...]\n");
        return;
    }

    const char *arquivoVC = argv[2];
    FILE *vc = NULL;
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Verifica se o arquivo .vc já existe
    if (access(arquivoVC, F_OK) == 0)
    {
        vc = fopen(arquivoVC, "r+b");
        if (!vc)
        {
            fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return;
        }
        if (carregarIndice(vc, &indice) < 0)
        {
            fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
            fclose(vc);
            destruirIndice(&indice);
            return;
        }
    }
    else
    {
        vc = fopen(arquivoVC, "w+b");
        if (!vc)
        {
            fprintf(stderr, "Erro ao criar o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return;
        }
        // Inicializa o arquivo com um índice vazio
        uint32_t numMembros = 0;
        if (fwrite(&numMembros, sizeof(uint32_t), 1, vc) != 1)
        {
            fprintf(stderr, "Erro ao inicializar o arquivo %s\n", arquivoVC);
            fclose(vc);
            destruirIndice(&indice);
            return;
        }
    }

    // Calcula a quantidade total de arquivos após a inserção
    uint32_t quantidade_total = indice.lista.quantidade + (argc - 3);
    // Calcula o novo tamanho do header (header + metadados de todos os arquivos)
    uint64_t novoHeader = sizeof(uint32_t) + (quantidade_total * sizeof(ArquivoMembro));
    // Realoca dados dos arquivos antigos se necessário
    uint64_t offsetDados = novoHeader;
    No *atual = indice.lista.primeiro;
    while (atual) {
        if (atual->arquivo.offset != offsetDados) {
            moverDados(vc, atual->arquivo.offset, offsetDados, atual->arquivo.emDisco);
            atual->arquivo.offset = offsetDados;
        }
        offsetDados += atual->arquivo.emDisco;
        atual = atual->proximo;
    }

    // Processa cada arquivo novo
    for (int i = 3; i < argc; i++)
    {
        ArquivoMembro novoMembro;
        strncpy(novoMembro.nome, argv[i], sizeof(novoMembro.nome) - 1);
        novoMembro.nome[sizeof(novoMembro.nome) - 1] = '\0';
        novoMembro.uid = indice.lista.quantidade + 1;
        novoMembro.modificacao = time(NULL);
        novoMembro.ordem = indice.lista.quantidade;
        novoMembro.offset = offsetDados;

        uint32_t tamanhoFinal = 0;
        if (inserirArquivo(argv[i], vc, &offsetDados, &tamanhoFinal, compress) != 0)
        {
            fprintf(stderr, "Erro ao inserir os dados do arquivo %s no .vc\n", argv[i]);
            continue;
        }
        novoMembro.tamanho = tamanhoFinal;
        novoMembro.emDisco = tamanhoFinal;

        adicionarAoIndice(&indice, novoMembro);
        printf("Arquivo '%s' adicionado ao índice e ao .vc com sucesso.\n", argv[i]);
    }

    // Salva apenas o índice atualizado
    rewind(vc);
    if (salvarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
    }

    destruirIndice(&indice);
    fclose(vc);
}


// Remove um arquivo do arquivador .vc
// Os arquivos restantes são movidos para preencher o espaço
void vinac_remove(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Uso: vinac -r arquivo.vc arquivo_a_remover\n");
        return;
    }

    const char *arquivoVC = argv[2];
    FILE *vc = fopen(arquivoVC, "r+b");
    if (!vc)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    IndiceArquivador indice;
    inicializarIndice(&indice);

    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        destruirIndice(&indice);
        return;
    }

    if (removerArquivo(vc, &indice.lista, argv[3]))
    {
        printf("Arquivo '%s' removido com sucesso.\n", argv[3]);

        // Atualiza o índice após a remoção
        rewind(vc);
        if (salvarIndice(vc, &indice) < 0)
        {
            fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
        }
    }
    else
    {
        fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", argv[3]);
    }

    destruirIndice(&indice);
    fclose(vc);
}

// Lista o conteúdo do arquivador .vc
void vinac_list(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Uso: vinac -c arquivo.vc\n");
        return;
    }

    const char *arquivoVC = argv[2];
    FILE *vc = fopen(arquivoVC, "rb");
    if (!vc)
    {
        fprintf(stderr, "Erro: arquivo %s não existe\n", arquivoVC);
        return;
    }

    IndiceArquivador indice;
    inicializarIndice(&indice);

    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Mostra a listagem normal dos arquivos
    listarArquivos(&indice);

    // Calcula o tamanho do header (metadados)
    uint64_t tamanhoHeader = sizeof(uint32_t) + (indice.lista.quantidade * sizeof(ArquivoMembro));

    // Mostra o layout de memória
    mostrarLayoutMemoria(&indice, tamanhoHeader);

    destruirIndice(&indice);
    fclose(vc);
}

// Extrai um ou mais arquivos do arquivador .vc
void vinac_extract(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Uso: vinac -x arquivo.vc arquivos...\n");
        return;
    }

    const char *arquivoVC = argv[2];
    FILE *vc = fopen(arquivoVC, "rb");
    if (!vc)
    {
        fprintf(stderr, "Erro: arquivo %s não existe\n", arquivoVC);
        return;
    }

    IndiceArquivador indice;
    inicializarIndice(&indice);

    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Processa cada arquivo especificado para extração
    for (int i = 3; i < argc; i++)
    {
        const char *nomeExtrair = argv[i];
        No *arquivo = buscarArquivo(&indice.lista, nomeExtrair);

        if (arquivo)
        {
            if (extrairArquivo(vc, arquivo->arquivo, ".") == 0)
            {
                printf("Arquivo '%s' extraído com sucesso.\n", nomeExtrair);
            }
            else
            {
                fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", nomeExtrair);
            }
        }
        else
        {
            fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", nomeExtrair);
        }
    }

    destruirIndice(&indice);
    fclose(vc);
}

// Move a posição lógica de um arquivo dentro do arquivador
// Útil para reorganizar a ordem dos arquivos no índice
void vinac_move(int argc, char *argv[])
{
    if (argc < 5)
    {
        fprintf(stderr, "Uso: vinac -m arquivo.vc posicao_atual nova_posicao\n");
        return;
    }

    const char *arquivoVC = argv[2];
    FILE *vc = fopen(arquivoVC, "r+b");
    if (!vc)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    uint32_t posicaoAtual = (uint32_t)atoi(argv[3]);
    uint32_t novaPosicao = (uint32_t)atoi(argv[4]);

    IndiceArquivador indice;
    inicializarIndice(&indice);

    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Valida as posições fornecidas
    if (posicaoAtual >= indice.lista.quantidade || novaPosicao >= indice.lista.quantidade)
    {
        fprintf(stderr, "Posições inválidas. O índice deve estar entre 0 e %d.\n",
                indice.lista.quantidade - 1);
        destruirIndice(&indice);
        fclose(vc);
        return;
    }

    // Encontra os nós das posições especificadas
    No *noAtual = indice.lista.primeiro;
    No *noNovo = indice.lista.primeiro;

    for (uint32_t i = 0; i < posicaoAtual && noAtual; i++)
    {
        noAtual = noAtual->proximo;
    }

    for (uint32_t i = 0; i < novaPosicao && noNovo; i++)
    {
        noNovo = noNovo->proximo;
    }

    if (!noAtual || !noNovo)
    {
        fprintf(stderr, "Erro ao encontrar as posições na lista.\n");
        destruirIndice(&indice);
        fclose(vc);
        return;
    }

    // Remove o nó da posição atual
    if (noAtual->anterior)
    {
        noAtual->anterior->proximo = noAtual->proximo;
    }
    else
    {
        indice.lista.primeiro = noAtual->proximo;
    }

    if (noAtual->proximo)
    {
        noAtual->proximo->anterior = noAtual->anterior;
    }
    else
    {
        indice.lista.ultimo = noAtual->anterior;
    }

    // Insere o nó na nova posição
    if (novaPosicao == 0)
    {
        noAtual->anterior = NULL;
        noAtual->proximo = indice.lista.primeiro;
        indice.lista.primeiro->anterior = noAtual;
        indice.lista.primeiro = noAtual;
    }
    else
    {
        noAtual->anterior = noNovo->anterior;
        noAtual->proximo = noNovo;
        noNovo->anterior->proximo = noAtual;
        noNovo->anterior = noAtual;
    }

    // Atualiza as ordens após a movimentação
    atualizarOrdens(&indice.lista);

    printf("Arquivo movido da posição %d para %d com sucesso.\n", posicaoAtual, novaPosicao);

    // Salva o índice atualizado
    rewind(vc);
    if (salvarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
    }

    destruirIndice(&indice);
    fclose(vc);
}