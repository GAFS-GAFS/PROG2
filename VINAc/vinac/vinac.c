#include "vinac.h"
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

    No *tmp = indice.lista.primeiro;

    size_t maiorArquivo = 0;
    while (tmp) {
        if (tmp->arquivo.tamanho > maiorArquivo)
            maiorArquivo = tmp->arquivo.tamanho;
        tmp = tmp->proximo;
    }

    while (atual)
    {
        if (atual->arquivo.offset != offsetDados)
        {
            moverDados(vc, atual->arquivo.offset, offsetDados, atual->arquivo.emDisco, maiorArquivo );
            atual->arquivo.offset = offsetDados;
        }
        offsetDados += atual->arquivo.emDisco;
        atual = atual->proximo;
    }

    // Processa cada arquivo novo
    for (int i = 3; i < argc; i++)
    {
        // Inserção plana
        ArquivoMembro novoMembro;
        strncpy(novoMembro.nome, argv[i], sizeof(novoMembro.nome) - 1);
        novoMembro.nome[sizeof(novoMembro.nome) - 1] = '\0';
        novoMembro.uid = indice.ultimoUID + 1;
        indice.ultimoUID = novoMembro.uid;
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

int vinac_insert_comp(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Uso: vinac -ic arquivo.vc arquivo1 [arquivo2 ...]\n");
        return -1;
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
            return -1;
        }
        if (carregarIndice(vc, &indice) < 0)
        {
            fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
            fclose(vc);
            destruirIndice(&indice);
            return -1;
        }
    }
    else
    {
        vc = fopen(arquivoVC, "w+b");
        if (!vc)
        {
            fprintf(stderr, "Erro ao criar o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return -1;
        }
        uint32_t numMembros = 0;
        if (fwrite(&numMembros, sizeof(uint32_t), 1, vc) != 1)
        {
            fprintf(stderr, "Erro ao inicializar o arquivo %s\n", arquivoVC);
            fclose(vc);
            destruirIndice(&indice);
            return -1;
        }
    }

    // Calcula a quantidade total de arquivos após a inserção
    uint32_t quantidade_total = indice.lista.quantidade;
    for (int i = 3; i < argc; i++) {
        No *existente = buscarArquivo(&indice.lista, argv[i]);
        if (!existente) quantidade_total++;
    }
    uint64_t novoHeader = sizeof(uint32_t) + (quantidade_total * sizeof(ArquivoMembro));
    uint64_t offsetDados = novoHeader;

    // Descobre o maior arquivo já presente no archive
    size_t maiorArquivo = 0;
    No *tmp = indice.lista.primeiro;
    while (tmp) {
        if (tmp->arquivo.tamanho > maiorArquivo)
            maiorArquivo = tmp->arquivo.tamanho;
        tmp = tmp->proximo;
    }

    // Realoca dados dos arquivos antigos se necessário
    No *atual = indice.lista.primeiro;
    while (atual)
    {
        if (atual->arquivo.offset != offsetDados)
        {
            moverDados(vc, atual->arquivo.offset, offsetDados, atual->arquivo.emDisco, maiorArquivo);

            atual->arquivo.offset = offsetDados;
        }
        offsetDados += atual->arquivo.emDisco;
        atual = atual->proximo;
    }

    // Para cada arquivo da linha de comando
    for (int i = 3; i < argc; i++)
    {
        No *existente = buscarArquivo(&indice.lista, argv[i]);
        int diffTamanho = 0;
        int novoEmDisco = 0;
        uint32_t tamOriginal = 0;
        uint64_t offset = 0;

        if (existente)
        {
            // Substituição: mantém ordem, atualiza metadados, atualiza offsets dos seguintes
            offset = existente->arquivo.offset;
            // Aloca buffer temporário para leitura e compressão
            size_t bufferSize = existente->arquivo.tamanho > 4096 ? existente->arquivo.tamanho : 4096;
            unsigned char *buffer = malloc(bufferSize);
            if (!buffer)
            {
                fprintf(stderr, "Erro de memória para buffer.\n");
                continue;
            }
            novoEmDisco = inserirComp(argv[i], vc, offset, existente->arquivo.emDisco, 1, buffer, bufferSize, &diffTamanho, &tamOriginal);
            free(buffer);
            if (novoEmDisco < 0)
            {
                fprintf(stderr, "Erro ao substituir o arquivo %s\n", argv[i]);
                continue;
            }
            if (diffTamanho != 0)
            {
                existente->arquivo.emDisco = novoEmDisco;
                existente->arquivo.tamanho = tamOriginal;
                existente->arquivo.modificacao = time(NULL);
                // Atualiza offsets dos arquivos seguintes
                No *seg = existente->proximo;
                while (seg)
                {
                    seg->arquivo.offset += diffTamanho;
                    seg = seg->proximo;
                }
                printf("Arquivo '%s' comprimido/substituído no .vc.\n", argv[i]);
            }
            else
            {
                existente->arquivo.emDisco = novoEmDisco;
                existente->arquivo.tamanho = tamOriginal;
                existente->arquivo.modificacao = time(NULL);
                printf("Arquivo '%s' já está otimizado (compressão não reduz tamanho).\n", argv[i]);
            }
        }
        else
        {
            // Inserção ao final: calcula offset correto
            offset = offsetDados;
            // Descobre tamanho do arquivo para buffer
            FILE *arq = fopen(argv[i], "rb");
            size_t bufferSize = maiorArquivo;
            if (arq) {
                fseek(arq, 0, SEEK_END);
                size_t tamArq = ftell(arq);
                if (tamArq > bufferSize) bufferSize = tamArq;
                fclose(arq);
            }
            if (bufferSize == 0) bufferSize = 4096;
            unsigned char *buffer = malloc(bufferSize);
            if (!buffer)
            {
                fprintf(stderr, "Erro de memória para buffer.\n");
                continue;
            }
            novoEmDisco = inserirComp(argv[i], vc, offset, 0, 1, buffer, bufferSize, &diffTamanho, &tamOriginal);
            free(buffer);
            if (novoEmDisco < 0)
            {
                fprintf(stderr, "Erro ao inserir arquivo novo %s\n", argv[i]);
                continue;
            }
            ArquivoMembro novoMembro;
            strncpy(novoMembro.nome, argv[i], sizeof(novoMembro.nome) - 1);
            novoMembro.nome[sizeof(novoMembro.nome) - 1] = '\0';
            novoMembro.uid = indice.ultimoUID + 1;
            indice.ultimoUID = novoMembro.uid;
            novoMembro.tamanho = tamOriginal;
            novoMembro.emDisco = (uint32_t)novoEmDisco;
            novoMembro.modificacao = time(NULL);
            novoMembro.ordem = indice.lista.quantidade;
            novoMembro.offset = offset;
            adicionarAoIndice(&indice, novoMembro);
            printf("Arquivo '%s' inserido comprimido no .vc.\n", argv[i]);
            offsetDados += novoEmDisco;
        }
    }

    // Salva apenas o índice atualizado
    rewind(vc);
    if (salvarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
    }
    destruirIndice(&indice);
    fclose(vc);
    return 0;
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

    destruirIndice(&indice);
    fclose(vc);
}

// Extrai um ou mais arquivos do arquivador .vc
void vinac_extract(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Uso: vinac -x arquivo.vc [arquivos...]\n");
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

    // Se nenhum membro for especificado, extrai todos
    if (argc == 3)
    {
        No *atual = indice.lista.primeiro;
        while (atual)
        {
            if (extrairArquivo(vc, atual->arquivo, ".") == 0)
            {
                printf("Arquivo '%s' extraído com sucesso.\n", atual->arquivo.nome);
            }
            else
            {
                fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", atual->arquivo.nome);
            }
            atual = atual->proximo;
        }
    }
    else if (argc == 4 && strcmp(argv[3], "*") == 0)
    {
        No *atual = indice.lista.primeiro;
        while (atual)
        {
            if (extrairArquivo(vc, atual->arquivo, ".") == 0)
            {
                printf("Arquivo '%s' extraído com sucesso.\n", atual->arquivo.nome);
            }
            else
            {
                fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", atual->arquivo.nome);
            }
            atual = atual->proximo;
        }
    }
    else if (argc == 4)
    {
        // Se só um arquivo for especificado
        const char *nomeExtrair = argv[3];
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
    else
    {
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
    }

    destruirIndice(&indice);
    fclose(vc);
}

void vinac_move(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Uso: vinac -m arquivo.vc posicao_atual nova_posicao\n");
        return;
    }

    FILE *vc = fopen(argv[2], "r+b");
    if (!vc) {
        perror("Erro ao abrir arquivo");
        return;
    }

    IndiceArquivador idx;
    inicializarIndice(&idx);

    if (carregarIndice(vc, &idx) < 0) {
        fprintf(stderr, "Erro ao carregar índice.\n");
        fclose(vc);
        return;
    }

    uint32_t posAtual = atoi(argv[3]);
    uint32_t novaPos = atoi(argv[4]);

    if (posAtual == novaPos ||
        posAtual >= idx.lista.quantidade ||
        novaPos >= idx.lista.quantidade) {
        fprintf(stderr, "Posições inválidas.\n");
        destruirIndice(&idx);
        fclose(vc);
        return;
    }

    // Encontra tamanho do maior membro
    size_t maior = encontrarMaiorArquivo(&idx.lista, 0);

    unsigned char *buffer1 = malloc(maior);
    unsigned char *buffer2 = malloc(maior);
    if (!buffer1 || !buffer2) {
        perror("malloc");
        free(buffer1);
        free(buffer2);
        destruirIndice(&idx);
        fclose(vc);
        return;
    }

    // Localiza o nó a ser movido
    No *noAtual = idx.lista.primeiro;
    for (uint32_t i = 0; i < posAtual; i++)
        noAtual = noAtual->proximo;

    size_t tamMovido = noAtual->arquivo.emDisco;

    fseek(vc, noAtual->arquivo.offset, SEEK_SET);
    fread(buffer1, 1, tamMovido, vc);

    if (posAtual < novaPos) {
        No *tmp = noAtual->proximo;
        while (tmp && tmp->arquivo.ordem <= novaPos) {
            fseek(vc, tmp->arquivo.offset, SEEK_SET);
            fread(buffer2, 1, tmp->arquivo.emDisco, vc);

            fseek(vc, tmp->arquivo.offset - tamMovido, SEEK_SET);
            fwrite(buffer2, 1, tmp->arquivo.emDisco, vc);
            tmp->arquivo.offset -= tamMovido;

            tmp = tmp->proximo;
        }
        noAtual->arquivo.offset = tmp ? tmp->arquivo.offset : (uint64_t)ftell(vc);
        noAtual->arquivo.offset -= tamMovido;

    } else {
        No *tmp = noAtual->anterior;
        while (tmp && tmp->arquivo.ordem >= novaPos) {
            fseek(vc, tmp->arquivo.offset, SEEK_SET);
            fread(buffer2, 1, tmp->arquivo.emDisco, vc);

            fseek(vc, tmp->arquivo.offset + tamMovido, SEEK_SET);
            fwrite(buffer2, 1, tmp->arquivo.emDisco, vc);
            tmp->arquivo.offset += tamMovido;

            tmp = tmp->anterior;
        }
        if (tmp)
            noAtual->arquivo.offset = tmp->arquivo.offset + tmp->arquivo.emDisco;
        else
            noAtual->arquivo.offset = sizeof(uint32_t) + idx.lista.quantidade * sizeof(ArquivoMembro);
    }

    fseek(vc, noAtual->arquivo.offset, SEEK_SET);
    fwrite(buffer1, 1, tamMovido, vc);

    free(buffer1);
    free(buffer2);

    moverNoLista(&idx.lista, posAtual, novaPos);
    atualizarOrdens(&idx.lista);

    rewind(vc);
    if (salvarIndice(vc, &idx) < 0)
        fprintf(stderr, "Erro ao salvar índice atualizado.\n");

    destruirIndice(&idx);
    fclose(vc);
    printf("Arquivo movido corretamente.\n");
}
