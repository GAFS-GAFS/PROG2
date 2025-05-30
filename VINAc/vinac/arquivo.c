#include "arquivo.h"

#include "arquivo.h"

// Função auxiliar para encontrar o maior arquivo no archive
size_t encontrarMaiorArquivo(Lista *lista, size_t tamanhoAtual)
{
    size_t maiorTamanho = tamanhoAtual;

    No *atual = lista->primeiro;

    // Percorre a lista encadeada de membros
    while (atual != NULL)
    {
        // Verifica se o tamanho do membro atual é maior que o maior encontrado até agora
        if (atual->arquivo.tamanho > maiorTamanho)
        {
            maiorTamanho = atual->arquivo.tamanho;
        }
        atual = atual->proximo;
    }

    return (maiorTamanho); // Retorna o maior tamanho encontrado
}

// Função para mover dados dentro do arquivo .vc, respeitando o tamanho de buffer
int moverDados(FILE *vc, uint64_t offsetInicio, uint64_t offsetFim, size_t tamanho, size_t tamanhoBuffer)
{
    // Se o tamanho a mover é menor que o buffer, ajusta o tamanho do buffer
    if (tamanho < tamanhoBuffer)
        tamanhoBuffer = tamanho;

    // Aloca buffer temporário para movimentação dos dados
    unsigned char *buffer = malloc(tamanhoBuffer);

    // Verifica se malloc falhou
    if (!buffer)
        return (-1);

    // Se vamos mover os dados para frente no arquivo
    if (offsetFim > offsetInicio)
    {
        size_t bytesRestantes = tamanho;

        // Move os dados de trás para frente para evitar sobrescrita
        while (bytesRestantes > 0)
        {
            // Define o número de bytes a serem lidos neste ciclo
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;
            uint64_t posOrigem = offsetInicio + bytesRestantes - bytesLer;
            uint64_t posDestino = offsetFim + bytesRestantes - bytesLer;

            // Posiciona no início do bloco de origem
            fseek(vc, posOrigem, SEEK_SET);

            // Lê dados da origem
            if (fread(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer); // Libera buffer antes de sair
                return (-1);
            }

            // Posiciona no destino e grava
            fseek(vc, posDestino, SEEK_SET);

            // Escreve dados no destino
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer); // Libera buffer antes de sair
                return (-1);
            }

            // Atualiza o total de bytes restantes
            bytesRestantes -= bytesLer;
        }
    }
    else
    {
        // Caso o destino esteja antes da origem, movimenta de frente para trás
        size_t bytesRestantes = tamanho;

        while (bytesRestantes > 0)
        {
            // Define o número de bytes a serem lidos
            size_t bytesLer = (bytesRestantes < tamanhoBuffer) ? bytesRestantes : tamanhoBuffer;

            fseek(vc, offsetInicio, SEEK_SET);

            // Lê dados da origem
            if (fread(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer); // Libera buffer antes de sair
                return (-1);
            }

            fseek(vc, offsetFim, SEEK_SET);

            // Escreve dados no destino
            if (fwrite(buffer, 1, bytesLer, vc) != bytesLer)
            {
                free(buffer); // Libera buffer antes de sair
                return (-1);
            }

            // Atualiza os offsets para próxima parte
            offsetInicio += bytesLer;
            offsetFim += bytesLer;
            bytesRestantes -= bytesLer;
        }
    }

    // Libera o buffer após uso
    free(buffer);
    return (0); // Sucesso
}

// Função para inserir um novo arquivo no arquivo .vc
int inserirArquivo(const char *nomeArquivo, FILE *vc, uint64_t *offsetAtual, uint32_t *tamanhoFinal, uint32_t compressao)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo)
        return (-1);

    // Move para o final para obter o tamanho total
    fseek(arquivo, 0, SEEK_END);
    size_t tamanhoOriginal = ftell(arquivo);
    rewind(arquivo);

    // Aloca memória para ler o conteúdo do arquivo
    unsigned char *buffer = malloc(tamanhoOriginal);

    // Verifica se a alocação falhou
    if (!buffer)
    {
        fclose(arquivo);
        return (-1);
    }

    // Lê o arquivo inteiro para a memória
    if (fread(buffer, 1, tamanhoOriginal, arquivo) != tamanhoOriginal)
    {
        free(buffer);
        fclose(arquivo);
        return (-1);
    }

    // Fecha o arquivo original após leitura
    fclose(arquivo);

    // Por padrão, os dados finais serão os dados originais
    unsigned char *dadosFinais = buffer;
    *tamanhoFinal = (uint32_t)tamanhoOriginal;
    unsigned char *bufferComprimido = NULL;

    // Verifica se a compressão foi requisitada
    if (compressao)
    {
        bufferComprimido = malloc(tamanhoOriginal);

        // Verifica se a alocação do buffer comprimido falhou
        if (!bufferComprimido)
        {
            free(buffer);
            return (-1);
        }

        // Tenta comprimir os dados
        int tamanhoComprimido = LZ_Compress(buffer, bufferComprimido, (unsigned int)tamanhoOriginal);

        // Se a compressão foi bem-sucedida e útil (tamanho menor), usa os dados comprimidos
        if (tamanhoComprimido > 0 && (uint32_t)tamanhoComprimido < (uint32_t)tamanhoOriginal)
        {
            dadosFinais = bufferComprimido;
            *tamanhoFinal = (uint32_t)tamanhoComprimido;
        }
    }

    // Posiciona o ponteiro de escrita no arquivo .vc no local correto
    fseek(vc, *offsetAtual, SEEK_SET);

    // Escreve os dados finais (comprimidos ou não) no .vc
    size_t bytesEscritos = fwrite(dadosFinais, 1, *tamanhoFinal, vc);

    // Libera o buffer original
    free(buffer);

    // Libera buffer comprimido, se alocado
    if (bufferComprimido)
    {
        free(bufferComprimido);
    }

    // Atualiza o offset para o próximo arquivo a ser escrito
    *offsetAtual += *tamanhoFinal;

    // Retorna sucesso (0) apenas se todos os bytes foram escritos corretamente
    return ((bytesEscritos == *tamanhoFinal) ? 0 : -1);
}

// Função para inserir um novo arquivo comprimido, ou atualizar com compressão um arquivo no arquivo .vc
int inserirComp(const char *nomeArquivo, FILE *vc, uint64_t offset, uint32_t emDiscoAntigo, uint32_t compressao, unsigned char *buffer, size_t bufferSize, int *diffTamanho, uint32_t *tamanhoOriginal)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");

    // Verifica se o arquivo foi aberto com sucesso
    if (!arquivo)
        return (-1);

    // Obtém o tamanho original do arquivo
    fseek(arquivo, 0, SEEK_END);
    size_t tamOrig = ftell(arquivo);
    rewind(arquivo);

    // Verifica se o arquivo é maior que o buffer disponível
    if (tamOrig > bufferSize)
    {
        fclose(arquivo);
        return (-1);
    }

    // Lê o conteúdo completo do arquivo para o buffer
    if (fread(buffer, 1, tamOrig, arquivo) != tamOrig)
    {
        fclose(arquivo);
        return (-1);
    }

    // Fecha o arquivo após leitura
    fclose(arquivo);

    // Se o ponteiro tamanhoOriginal não for nulo, armazena o tamanho original nele
    if (tamanhoOriginal)
    {
        *tamanhoOriginal = (uint32_t)tamOrig;
    }

    // Por padrão, os dados finais são os dados originais lidos
    unsigned char *dadosFinais = buffer;
    uint32_t tamanhoFinal = (uint32_t)tamOrig;
    unsigned char *bufferComprimido = NULL;

    // Verifica se a compressão foi requisitada
    if (compressao)
    {
        // Aloca buffer para os dados comprimidos
        bufferComprimido = malloc(bufferSize);

        // Verifica se a alocação falhou
        if (!bufferComprimido)
            return (-1);

        // Realiza compressão usando LZ
        int tamanhoComprimido = LZ_Compress(buffer, bufferComprimido, (unsigned int)tamOrig);

        // Usa os dados comprimidos apenas se o tamanho final for menor que o original
        if (tamanhoComprimido > 0 && (uint32_t)tamanhoComprimido < (uint32_t)tamOrig)
        {
            dadosFinais = bufferComprimido;
            tamanhoFinal = (uint32_t)tamanhoComprimido;
        }
    }

    // Se diffTamanho não for nulo, armazena a diferença entre novo e antigo
    if (diffTamanho)
    {
        *diffTamanho = (int)tamanhoFinal - (int)emDiscoAntigo;
    }

    // Verifica se é uma substituição e o novo tamanho é menor que o antigo
    if (emDiscoAntigo > 0 && tamanhoFinal < emDiscoAntigo)
    {
        // Obtém o tamanho final atual do arquivo .vc
        fseek(vc, 0, SEEK_END);

        uint64_t fimArquivo = ftell(vc);
        uint64_t offsetOrigem = offset + emDiscoAntigo;
        uint64_t offsetDestino = offset + tamanhoFinal;

        // Calcula quantos bytes precisam ser movidos para frente
        size_t bytesMover = fimArquivo > offsetOrigem ? fimArquivo - offsetOrigem : 0;

        // Verifica se há bytes para mover
        if (bytesMover > 0)
        {
            size_t rest = 0;

            // Move os dados que estavam após o membro substituído
            while (rest < bytesMover)
            {
                // Calcula quantos bytes ler no ciclo atual
                size_t ler = (bytesMover - rest) < bufferSize ? (bytesMover - rest) : bufferSize;

                fseek(vc, offsetOrigem + rest, SEEK_SET);

                // Lê os dados da posição original
                if (fread(buffer, 1, ler, vc) != ler)
                {
                    // Libera o buffer comprimido se existir
                    if (bufferComprimido)
                    {
                        free(bufferComprimido);
                    }
                    return (-1);
                }

                fseek(vc, offsetDestino + rest, SEEK_SET);

                // Escreve os dados na nova posição (mais para frente)
                if (fwrite(buffer, 1, ler, vc) != ler)
                {
                    // Libera o buffer comprimido se existir
                    if (bufferComprimido)
                    {
                        free(bufferComprimido);
                    }
                    return (-1);
                }

                rest += ler; // Atualiza o total já movido
            }
        }

        // Trunca o arquivo no novo fim (removendo espaço não utilizado)
        if (ftruncate(fileno(vc), offsetDestino + bytesMover) != 0)
        {
            if (bufferComprimido)
            {
                free(bufferComprimido);
            }
            return (-1);
        }
    }
    // Caso o novo tamanho seja MAIOR que o antigo, será necessário mover os dados para trás
    else if (emDiscoAntigo > 0 && tamanhoFinal > emDiscoAntigo)
    {
        // Move para o final do arquivo
        fseek(vc, 0, SEEK_END);

        uint64_t fimArquivo = ftell(vc);
        uint64_t offsetOrigem = offset + emDiscoAntigo;
        uint64_t offsetDestino = offset + tamanhoFinal;

        // Calcula quantos bytes precisam ser movidos
        size_t bytesMover = fimArquivo > offsetOrigem ? fimArquivo - offsetOrigem : 0;

        if (bytesMover > 0)
        {
            size_t rest = bytesMover;

            // Move os dados de trás para frente para evitar sobrescrita
            while (rest > 0)
            {
                size_t ler = rest < bufferSize ? rest : bufferSize;
                fseek(vc, offsetOrigem + rest - ler, SEEK_SET);

                // Lê bloco de dados da origem
                if (fread(buffer, 1, ler, vc) != ler)
                {
                    if (bufferComprimido)
                    {
                        free(bufferComprimido);
                    }
                    return (-1);
                }

                fseek(vc, offsetDestino + rest - ler, SEEK_SET);

                // Escreve o bloco de dados na nova posição
                if (fwrite(buffer, 1, ler, vc) != ler)
                {
                    if (bufferComprimido)
                    {
                        free(bufferComprimido);
                    }
                    return (-1);
                }
                rest -= ler; // Atualiza a quantidade restante a mover
            }
        }
    }

    // Escreve os dados finais (comprimidos ou não) na posição correta
    fseek(vc, offset, SEEK_SET);

    // Verifica se todos os bytes foram escritos corretamente
    if (fwrite(dadosFinais, 1, tamanhoFinal, vc) != tamanhoFinal)
    {
        if (bufferComprimido)
        {
            free(bufferComprimido);
        }
        return (-1);
    }

    // Libera o buffer comprimido, se usado
    if (bufferComprimido)
    {
        free(bufferComprimido);
    }

    // Retorna o tamanho final escrito
    return ((int)tamanhoFinal);
}

// Função para extrair um arquivo do .vc
int extrairArquivo(FILE *vc, ArquivoMembro membro, const char *pastaDestino)
{
    char caminhoCompleto[2048];

    // Monta o caminho completo de extração usando pastaDestino + nome do membro
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s/%s", pastaDestino, membro.nome);

    // Abre o arquivo de destino para escrita binária
    FILE *destino = fopen(caminhoCompleto, "wb");

    // Verifica se o arquivo foi aberto corretamente
    if (!destino)
        return (-1);

    // Posiciona o ponteiro de leitura no offset do membro no arquivo .vc
    if (fseek(vc, membro.offset, SEEK_SET) != 0)
    {
        fclose(destino);
        return (-1);
    }

    // Verifica se o arquivo está comprimido (tamanho em disco é diferente do original)
    if (membro.emDisco != membro.tamanho)
    {
        // Aloca buffer para armazenar os dados comprimidos
        unsigned char *bufferComp = malloc(membro.emDisco);

        // Verifica se a alocação falhou
        if (!bufferComp)
        {
            fclose(destino);
            return (-1);
        }

        // Lê os dados comprimidos do .vc
        if (fread(bufferComp, 1, membro.emDisco, vc) != membro.emDisco)
        {
            free(bufferComp);
            fclose(destino);
            return (-1);
        }

        // Aloca buffer para descompressão (tamanho original do arquivo)
        unsigned char *bufferDecomp = malloc(membro.tamanho);

        // Verifica se a alocação falhou
        if (!bufferDecomp)
        {
            free(bufferComp);
            fclose(destino);
            return (-1);
        }

        // Realiza a descompressão dos dados
        LZ_Uncompress(bufferComp, bufferDecomp, membro.emDisco);

        // Escreve o conteúdo descomprimido no arquivo de destino
        fwrite(bufferDecomp, 1, membro.tamanho, destino);

        // Libera os buffers utilizados
        free(bufferComp);
        free(bufferDecomp);
    }
    else
    {
        // Caso o arquivo não esteja comprimido, apenas copia os dados em blocos
        uint32_t restante = membro.tamanho;

        // Aloca buffer para leitura
        unsigned char buffer[restante];

        // Enquanto ainda houver dados a copiar
        while (restante > 0)
        {
            // Define quantos bytes serão lidos neste ciclo
            size_t ler = (restante > sizeof(buffer)) ? sizeof(buffer) : restante;

            // Lê os dados do arquivo .vc
            size_t lidos = fread(buffer, 1, ler, vc);

            // Se nada foi lido, interrompe
            if (lidos == 0)
                break;

            // Escreve os dados no destino
            fwrite(buffer, 1, lidos, destino);
            restante -= lidos;
        }
    }

    // Fecha o arquivo extraído
    fclose(destino);
    return (0); // sucesso
}

// Função para remover um arquivo do arquivo .vc
int removerArquivo(FILE *vc, Lista *lista, const char *nome)
{
    No *atual = lista->primeiro;
    uint32_t posicao = 0;

    // Determina o maior tamanho de membro para ser usado como buffer na movimentação
    size_t tamanhoBuffer = encontrarMaiorArquivo(lista, 0);

    // Percorre a lista para encontrar o membro com nome correspondente
    while (atual != NULL)
    {
        // Compara o nome do membro com o nome desejado
        if (strcmp(atual->arquivo.nome, nome) == 0)
        {
            uint64_t offsetRemovido = atual->arquivo.offset;
            No *mover = atual->proximo;
            uint64_t novoOffset = offsetRemovido;

            // Move os arquivos seguintes para frente, sobrescrevendo o espaço do arquivo removido
            while (mover)
            {
                moverDados(vc, mover->arquivo.offset, novoOffset, mover->arquivo.emDisco, tamanhoBuffer);
                mover->arquivo.offset = novoOffset;
                novoOffset += mover->arquivo.emDisco;
                mover = mover->proximo;
            }

            // Armazena a posição do nó a ser removido
            uint32_t posRemovida = posicao;

            // Remove o nó da lista encadeada (ajusta ponteiros)
            if (atual->anterior)
            {
                atual->anterior->proximo = atual->proximo;
            }
            else
            {
                lista->primeiro = atual->proximo;
            }

            if (atual->proximo)
            {
                atual->proximo->anterior = atual->anterior;
            }
            else
            {
                lista->ultimo = atual->anterior;
            }

            // Libera o nó removido
            free(atual);

            // Atualiza a quantidade de membros
            lista->quantidade--;

            // Atualiza o campo "ordem" dos membros restantes
            atualizarOrdens(lista);

            // Reposiciona o ponteiro no início do arquivo .vc
            rewind(vc);
            uint32_t numMembros = 0;

            // Lê o número total de membros no início do arquivo
            fread(&numMembros, sizeof(uint32_t), 1, vc);

            // Verifica se é necessário reordenar os metadados no cabeçalho
            if (numMembros > 0 && posRemovida < numMembros)
            {
                // Move todos os metadados após o membro removido uma posição para cima
                for (uint32_t i = posRemovida + 1; i < numMembros; i++)
                {
                    ArquivoMembro temp;

                    fseek(vc, sizeof(uint32_t) + i * sizeof(ArquivoMembro), SEEK_SET);
                    fread(&temp, sizeof(ArquivoMembro), 1, vc);
                    fseek(vc, sizeof(uint32_t) + (i - 1) * sizeof(ArquivoMembro), SEEK_SET);
                    fwrite(&temp, sizeof(ArquivoMembro), 1, vc);
                }

                // Atualiza o número de membros
                numMembros--;
                rewind(vc);
                fwrite(&numMembros, sizeof(uint32_t), 1, vc);

                // Calcula o novo tamanho da área de diretório (cabeçalho)
                uint64_t tamanhoHeader = sizeof(uint32_t) + (numMembros) * sizeof(ArquivoMembro);
                uint64_t fimDados = 0;

                // Verifica até onde vai a área de dados
                No *ult = lista->ultimo;

                if (ult)
                {
                    fimDados = ult->arquivo.offset + ult->arquivo.emDisco;
                }

                // O novo tamanho do arquivo é o maior entre fim dos dados ou fim do cabeçalho
                uint64_t novoTamanho = (tamanhoHeader > fimDados) ? tamanhoHeader : fimDados;

                // Trunca o arquivo no novo tamanho total
                ftruncate(fileno(vc), novoTamanho);
            }

            return (1); // remoção realizada com sucesso
        }

        // Vai para o próximo nó
        atual = atual->proximo;
        posicao++;
    }

    return (0); // membro não encontrado
}