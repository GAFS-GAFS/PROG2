#include "arquivo.h"

// Função auxiliar para encontrar o maior arquivo no archive
size_t encontrarMaiorArquivo(Lista *lista, size_t tamanhoAtual)
{
    size_t maiorTamanho = tamanhoAtual;

    No *atual = lista->primeiro;
    while (atual != NULL)
    {
        if (atual->arquivo.tamanho > maiorTamanho)
        {
            maiorTamanho = atual->arquivo.tamanho;
        }
        atual = atual->proximo;
    }

    return maiorTamanho;
}

// Move um bloco de dados dentro do arquivo
int moverDados(FILE *vc, uint64_t offsetInicio, uint64_t offsetFim, size_t tamanho)
{
    // Encontra tamanho do maior arquivo para alocar buffer
    rewind(vc);
    uint32_t numMembros;
    fread(&numMembros, sizeof(uint32_t), 1, vc);

    // Aloca buffer para ler os metadados
    Lista lista;
    inicializarLista(&lista);

    // Lê os metadados dos arquivos
    for (uint32_t i = 0; i < numMembros; i++)
    {
        ArquivoMembro membro;
        fread(&membro, sizeof(ArquivoMembro), 1, vc);
        inserirArquivoLista(&lista, membro);
    }

    size_t tamanhoBuffer = encontrarMaiorArquivo(&lista, tamanho);
    unsigned char *buffer = malloc(tamanhoBuffer);

    if (!buffer)
    {
        destruirLista(&lista);
        return -1;
    }

    // Se movendo para frente, começa do fim
    if (offsetFim > offsetInicio)
    {
        size_t bytesRestantes = tamanho;
        while (bytesRestantes > 0)
        {
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;

            fseek(vc, offsetInicio + bytesRestantes - bytesLer, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }

            fseek(vc, offsetFim + bytesRestantes - bytesLer, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }

            bytesRestantes -= bytesLer;
        }
    }
    else
    {
        // Se movendo para trás, começa do início
        size_t bytesRestantes = tamanho;
        while (bytesRestantes > 0)
        {
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;

            fseek(vc, offsetInicio, SEEK_SET);
            if (fread(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer);
                destruirLista(&lista);
                return -1;
            }

            fseek(vc, offsetFim, SEEK_SET);
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer)
            {
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
int inserirArquivo(const char *nomeArquivo, FILE *vc, uint64_t *offsetAtual, uint32_t *tamanhoFinal, uint32_t compressao)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo)
        return -1;

    // Obtém o tamanho do arquivo
    fseek(arquivo, 0, SEEK_END);
    size_t tamanhoOriginal = ftell(arquivo);
    rewind(arquivo);

    // Lê o arquivo fonte
    unsigned char *buffer = malloc(tamanhoOriginal);
    if (!buffer)
    {
        fclose(arquivo);
        return -1;
    }

    // Lê o arquivo fonte para o buffer
    if (fread(buffer, 1, tamanhoOriginal, arquivo) != tamanhoOriginal)
    {
        free(buffer);
        fclose(arquivo);
        return -1;
    }
    fclose(arquivo);

    // Prepara os dados finais (comprimidos ou não)
    unsigned char *dadosFinais = buffer;
    *tamanhoFinal = (uint32_t)tamanhoOriginal;
    unsigned char *bufferComprimido = NULL;

    if (compressao)
    {
        // Tenta comprimir os dados
        bufferComprimido = malloc(tamanhoOriginal);
        if (!bufferComprimido)
        {
            free(buffer);
            return -1;
        }

        int tamanhoComprimido = LZ_Compress(buffer, bufferComprimido, (unsigned int)tamanhoOriginal);
        if (tamanhoComprimido > 0 && (uint32_t)tamanhoComprimido < (uint32_t)tamanhoOriginal)
        {
            dadosFinais = bufferComprimido;
            *tamanhoFinal = (uint32_t)tamanhoComprimido;
        }
    }

    // Posiciona no offset atual correto e escreve os dados
    fseek(vc, *offsetAtual, SEEK_SET);
    size_t bytesEscritos = fwrite(dadosFinais, 1, *tamanhoFinal, vc);

    // Limpa a memória
    free(buffer);
    if (bufferComprimido)
        free(bufferComprimido);

    // Atualiza o offset para o próximo arquivo
    *offsetAtual += *tamanhoFinal;

    // Retorna sucesso apenas se escreveu todos os bytes
    return (bytesEscritos == *tamanhoFinal) ? 0 : -1;
}

// Manipula apenas os dados binários dentro do archive.vc
// Não altera índice, header ou metadados fora dos dados binários
// Recebe o offset de escrita, tamanho do espaço disponível (emDiscoAntigo, se substituição), e retorna o tamanho final escrito
// compressao: 1 para comprimir, 0 para não comprimir
// buffer: buffer de trabalho já alocado, de tamanho bufferSize (maior arquivo)
// Retorna tamanho final escrito (>0) ou -1 em erro
int substituirOuInserirArquivo(const char *nomeArquivo, FILE *vc, uint64_t offset, uint32_t emDiscoAntigo, uint32_t compressao, unsigned char *buffer, size_t bufferSize, int *diffTamanho, uint32_t *tamanhoOriginal)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo)
        return -1;

    fseek(arquivo, 0, SEEK_END);
    size_t tamOrig = ftell(arquivo);
    rewind(arquivo);
    if (tamOrig > bufferSize)
    {
        fclose(arquivo);
        return -1;
    }
    if (fread(buffer, 1, tamOrig, arquivo) != tamOrig)
    {
        fclose(arquivo);
        return -1;
    }
    fclose(arquivo);
    if (tamanhoOriginal)
        *tamanhoOriginal = (uint32_t)tamOrig;

    unsigned char *dadosFinais = buffer;
    uint32_t tamanhoFinal = (uint32_t)tamOrig;
    unsigned char *bufferComprimido = NULL;

    if (compressao)
    {
        bufferComprimido = malloc(bufferSize);
        if (!bufferComprimido)
            return -1;
        int tamanhoComprimido = LZ_Compress(buffer, bufferComprimido, (unsigned int)tamOrig);
        if (tamanhoComprimido > 0 && (uint32_t)tamanhoComprimido < (uint32_t)tamOrig)
        {
            dadosFinais = bufferComprimido;
            tamanhoFinal = (uint32_t)tamanhoComprimido;
        }
    }

    if (diffTamanho)
        *diffTamanho = (int)tamanhoFinal - (int)emDiscoAntigo;

    // Se substituição e novo tamanho < antigo, mover dados seguintes para frente
    if (emDiscoAntigo > 0 && tamanhoFinal < emDiscoAntigo)
    {
        fseek(vc, 0, SEEK_END);
        uint64_t fimArquivo = ftell(vc);
        uint64_t offsetOrigem = offset + emDiscoAntigo;
        uint64_t offsetDestino = offset + tamanhoFinal;
        size_t bytesMover = fimArquivo > offsetOrigem ? fimArquivo - offsetOrigem : 0;
        if (bytesMover > 0)
        {
            size_t rest = bytesMover;
            while (rest > 0)
            {
                size_t ler = rest < bufferSize ? rest : bufferSize;
                fseek(vc, offsetOrigem + rest - ler, SEEK_SET);
                fread(buffer, 1, ler, vc);
                fseek(vc, offsetDestino + rest - ler, SEEK_SET);
                fwrite(buffer, 1, ler, vc);
                rest -= ler;
            }
        }
        ftruncate(fileno(vc), offsetDestino + bytesMover);
    }
    else if (emDiscoAntigo > 0 && tamanhoFinal > emDiscoAntigo)
    {
        fseek(vc, 0, SEEK_END);
        uint64_t fimArquivo = ftell(vc);
        uint64_t offsetOrigem = offset + emDiscoAntigo;
        uint64_t offsetDestino = offset + tamanhoFinal;
        size_t bytesMover = fimArquivo > offsetOrigem ? fimArquivo - offsetOrigem : 0;
        if (bytesMover > 0)
        {
            size_t rest = bytesMover;
            while (rest > 0)
            {
                size_t ler = rest < bufferSize ? rest : bufferSize;
                fseek(vc, offsetOrigem + rest - ler, SEEK_SET);
                fread(buffer, 1, ler, vc);
                fseek(vc, offsetDestino + rest - ler, SEEK_SET);
                fwrite(buffer, 1, ler, vc);
                rest -= ler;
            }
        }
    }
    fseek(vc, offset, SEEK_SET);
    fwrite(dadosFinais, 1, tamanhoFinal, vc);
    if (bufferComprimido)
        free(bufferComprimido);
    return (int)tamanhoFinal;
}

int extrairArquivo(FILE *vc, ArquivoMembro membro, const char *pastaDestino)
{
    char caminhoCompleto[2048];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", pastaDestino, membro.nome);

    FILE *destino = fopen(caminhoCompleto, "wb");
    if (!destino)
        return -1;

    // Encontra tamanho do maior arquivo para buffer
    rewind(vc);
    uint32_t numMembros;
    fread(&numMembros, sizeof(uint32_t), 1, vc);

    Lista lista;
    inicializarLista(&lista);

    for (uint32_t i = 0; i < numMembros; i++)
    {
        ArquivoMembro membroTemp;
        fread(&membroTemp, sizeof(ArquivoMembro), 1, vc);
        inserirArquivoLista(&lista, membroTemp);
    }

    size_t tamanhoBuffer = encontrarMaiorArquivo(&lista, membro.tamanho);
    unsigned char *buffer = malloc(tamanhoBuffer);

    if (!buffer)
    {
        destruirLista(&lista);
        fclose(destino);
        return -1;
    }

    fseek(vc, membro.offset, SEEK_SET);

    if (membro.emDisco != membro.tamanho)
    {
        // Arquivo está comprimido
        if (fread(buffer, 1, membro.emDisco, vc) != membro.emDisco)
        {
            free(buffer);
            destruirLista(&lista);
            fclose(destino);
            return -1;
        }

        // Descomprime
        unsigned char *decompBuf = malloc(membro.tamanho);
        if (!decompBuf)
        {
            free(buffer);
            destruirLista(&lista);
            fclose(destino);
            return -1;
        }

        LZ_Uncompress(buffer, decompBuf, membro.emDisco);
        fwrite(decompBuf, 1, membro.tamanho, destino);
        free(decompBuf);
    }
    else
    {
        // Arquivo não comprimido
        if (fread(buffer, 1, membro.tamanho, vc) != membro.tamanho)
        {
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

int removerArquivo(FILE *vc, Lista *lista, const char *nome)
{
    No *atual = lista->primeiro;
    uint32_t posicao = 0;
    while (atual != NULL)
    {
        if (strcmp(atual->arquivo.nome, nome) == 0)
        {
            uint64_t offsetRemovido = atual->arquivo.offset;
            uint32_t tamanhoRemovido = atual->arquivo.emDisco;
            No *mover = atual->proximo;
            uint64_t novoOffset = offsetRemovido;

            // Move os arquivos seguintes para frente e atualiza offsets
            while (mover)
            {
                moverDados(vc, mover->arquivo.offset, novoOffset, mover->arquivo.emDisco);
                mover->arquivo.offset = novoOffset;
                novoOffset += mover->arquivo.emDisco;
                mover = mover->proximo;
            }

            // Remove o nó da lista
            uint32_t posRemovida = posicao;
            if (atual->anterior)
                atual->anterior->proximo = atual->proximo;
            else
                lista->primeiro = atual->proximo;

            if (atual->proximo)
                atual->proximo->anterior = atual->anterior;
            else
                lista->ultimo = atual->anterior;

            free(atual);
            lista->quantidade--;
            atualizarOrdens(lista);

            // --- REMOVE A STRUCT DO HEADER NO ARQUIVO .VC ---
            rewind(vc);
            uint32_t numMembros = 0;
            fread(&numMembros, sizeof(uint32_t), 1, vc);

            if (numMembros > 0 && posRemovida < numMembros)
            {
                // Move structs seguintes para cima
                for (uint32_t i = posRemovida + 1; i < numMembros; i++)
                {
                    ArquivoMembro temp;
                    fseek(vc, sizeof(uint32_t) + i * sizeof(ArquivoMembro), SEEK_SET);
                    fread(&temp, sizeof(ArquivoMembro), 1, vc);
                    fseek(vc, sizeof(uint32_t) + (i - 1) * sizeof(ArquivoMembro), SEEK_SET);
                    fwrite(&temp, sizeof(ArquivoMembro), 1, vc);
                }
                numMembros--;
                rewind(vc);
                fwrite(&numMembros, sizeof(uint32_t), 1, vc);

                // --- TRUNCA O HEADER PARA NÃO DEIXAR STRUCTS FANTASMAS ---
                uint64_t tamanhoHeader = sizeof(uint32_t) + (numMembros) * sizeof(ArquivoMembro);
                uint64_t fimDados = 0;
                No *ult = lista->ultimo;
                if (ult)
                    fimDados = ult->arquivo.offset + ult->arquivo.emDisco;
                uint64_t novoTamanho = (tamanhoHeader > fimDados) ? tamanhoHeader : fimDados;
                ftruncate(fileno(vc), novoTamanho);
            }

            return 1;
        }
        atual = atual->proximo;
        posicao++;
    }
    return 0;
}
