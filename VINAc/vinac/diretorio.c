#include "diretorio.h"

// Inicializa um novo índice vazio
void inicializarIndice(IndiceArquivador *idx)
{
    inicializarLista(&idx->lista);
    idx->ultimoUID = 0;
}

// Libera toda a memória alocada pelo índice
void destruirIndice(IndiceArquivador *idx)
{
    destruirLista(&idx->lista);
}

// Adiciona um novo arquivo ao índice ou atualiza se já existir
// Mantém a ordem original se o arquivo já existe
void adicionarAoIndice(IndiceArquivador *idx, ArquivoMembro membro)
{
    if (!idx)
        return;

    // Procura se o arquivo já existe para manter o UID
    No *existente = buscarArquivo(&idx->lista, membro.nome);

    if (existente)
    {
        // Mantém a ordem original e atualiza metadados
        membro.ordem = existente->arquivo.ordem;
        removerArquivoLista(&idx->lista, membro.nome);
    }

    inserirArquivoLista(&idx->lista, membro);
}

// Carrega os metadados do arquivo .vc para a RAM
// Lê a quantidade de membros e seus metadados do início do arquivo
int carregarIndice(FILE *vc, IndiceArquivador *idx)
{
    if (!vc || !idx)
        return (-1);

    rewind(vc);

    // Lê quantidade de membros
    uint32_t numMembros;

    if (fread(&numMembros, sizeof(uint32_t), 1, vc) != 1)
    {
        inicializarLista(&idx->lista);
        idx->ultimoUID = 0;
        return 0;
    }

    // Limpa lista atual se existir
    destruirLista(&idx->lista);
    inicializarLista(&idx->lista);

    idx->ultimoUID = 0;

    // Lê cada membro e adiciona à lista
    for (uint32_t i = 0; i < numMembros; i++)
    {
        ArquivoMembro membro;

        if (fread(&membro, sizeof(ArquivoMembro), 1, vc) != 1)
        {
            destruirLista(&idx->lista);
            idx->ultimoUID = 0;
            return -1;
        }

        inserirArquivoLista(&idx->lista, membro);

        if (membro.uid > idx->ultimoUID)
        {
            idx->ultimoUID = membro.uid;
        }
    }

    return (0);
}

// Salva o índice atual da RAM de volta no arquivo .vc
// Escreve primeiro a quantidade de membros, seguida pelos metadados
int salvarIndice(FILE *vc, IndiceArquivador *idx)
{
    if (!vc || !idx)
        return (-1);

    rewind(vc);

    // Escreve quantidade de membros
    if (fwrite(&idx->lista.quantidade, sizeof(uint32_t), 1, vc) != 1)
    {
        return (-1);
    }

    // Escreve cada membro da lista sequencialmente
    No *atual = idx->lista.primeiro;

    for (uint32_t i = 0; i < idx->lista.quantidade; i++)
    {
        if (!atual)
            return (-1);

        if (fwrite(&atual->arquivo, sizeof(ArquivoMembro), 1, vc) != 1)
        {
            return (-1);
        }

        atual = atual->proximo;
    }

    fflush(vc);

    return (0);
}

// Lista todos os arquivos presentes no índice com seus metadados
// Mostra: ordem, nome, UID, tamanho original, tamanho em disco e data

void listarArquivos(const IndiceArquivador *idx)
{
    if (!idx || !idx->lista.primeiro)
    {
        printf("Nenhum arquivo no índice.\n");
        return;
    }

    printf("Conteúdo do arquivo:\n");
    printf("Ordem  Nome                Uid      Tam.Orig  Tam.Disco  Data Mod.\n");
    printf("-----  ------------------- -------- --------- ---------- ----------\n");

    No *atual = idx->lista.primeiro;

    while (atual != NULL)
    {
        ArquivoMembro *m = &atual->arquivo;

        // Validação básica dos dados antes de imprimir
        if (!m->nome[0] || m->tamanho == 0)
        {
            atual = atual->proximo;
            continue;
        }

        // Formata a data para exibição
        char data[20] = "N/A";
        if (m->modificacao > 0)
        {
            struct tm *timeinfo = localtime(&m->modificacao);
            if (timeinfo)
            {
                strftime(data, sizeof(data), "%Y-%m-%d", timeinfo);
            }
        }

        // Imprime os dados do arquivo formatados
        printf("%-5u  %-19s %-8u %-9u %-10u %s\n",
               m->ordem,
               m->nome,
               m->uid,
               m->tamanho,
               m->emDisco,
               data);

        atual = atual->proximo;
    }

    printf("\nTotal de arquivos: %u\n", idx->lista.quantidade);
}

int moverNoLista(Lista *lista, uint32_t atualPos, uint32_t novaPos)
{
    if (atualPos == novaPos || !lista || atualPos >= lista->quantidade || novaPos >= lista->quantidade)
        return (-1);

    No *atual = lista->primeiro;

    for (uint32_t i = 0; i < atualPos; i++)
    {
        atual = atual->proximo;
    }

    // Remove nó
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

    // Insere nó
    No *novo = lista->primeiro;
    if (novaPos == 0)
    {
        atual->proximo = lista->primeiro;
        atual->anterior = NULL;
        lista->primeiro->anterior = atual;
        lista->primeiro = atual;
    }
    else
    {
        for (uint32_t i = 0; i < novaPos - 1; i++)
        {
            novo = novo->proximo;
        }

        atual->proximo = novo->proximo;
        atual->anterior = novo;

        if (novo->proximo)
        {
            novo->proximo->anterior = atual;
        }

        novo->proximo = atual;

        if (!atual->proximo)
        {
            lista->ultimo = atual;
        }
    }

    atualizarOrdens(lista);
    return (0);
}