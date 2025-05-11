#include "vinac.h"

// Função para inserir arquivos no arquivador .vc
void vinac_insert(int argc, char *argv[], uint32_t compress)
{
    // Verifica se há argumentos suficientes: pelo menos 4 (vinac, -i[c], arquivo.vc, arquivo1)
    if (argc < 4)
    {
        fprintf(stderr, "Uso: vinac -i[c] arquivo.vc arquivo1 [arquivo2 ...]\n");
        return;
    }

    // Obtém o nome do arquivo .vc da linha de comando
    const char *arquivoVC = argv[2];
    FILE *vc = NULL;
    IndiceArquivador indice;

    // Inicializa o índice em memória
    inicializarIndice(&indice);

    // Verifica se o arquivo .vc já existe
    if (access(arquivoVC, F_OK) == 0)
    {
        // Abre o arquivo existente para leitura e escrita binária
        vc = fopen(arquivoVC, "r+b");

        // Verifica se o arquivo foi aberto com sucesso
        if (!vc)
        {
            fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return;
        }

        // Carrega o índice atual do arquivo .vc para memória
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
        // Caso o arquivo .vc não exista, cria um novo
        vc = fopen(arquivoVC, "w+b");

        // Verifica se conseguiu criar o arquivo
        if (!vc)
        {
            fprintf(stderr, "Erro ao criar o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return;
        }

        // Escreve o valor inicial 0 para o número de membros no início do arquivo
        uint32_t numMembros = 0;

        // Verifica se conseguiu escrever corretamente
        if (fwrite(&numMembros, sizeof(uint32_t), 1, vc) != 1)
        {
            fprintf(stderr, "Erro ao inicializar o arquivo %s\n", arquivoVC);
            fclose(vc);
            destruirIndice(&indice);
            return;
        }
    }

    // Calcula o número total de membros após a inserção dos novos arquivos
    uint32_t quantidade_total = indice.lista.quantidade + (argc - 3);

    // Calcula o novo tamanho da área de diretório (header)
    uint64_t novoHeader = sizeof(uint32_t) + (quantidade_total * sizeof(ArquivoMembro));

    // Define o novo offset onde os dados reais começarão
    uint64_t offsetDados = novoHeader;

    No *atual = indice.lista.primeiro;
    No *tmp = indice.lista.primeiro;

    size_t maiorArquivo = 0;

    // Encontra o maior arquivo no índice atual (usado para definir o tamanho do buffer)
    while (tmp)
    {
        if (tmp->arquivo.tamanho > maiorArquivo)
        {
            maiorArquivo = tmp->arquivo.tamanho;
        }
        tmp = tmp->proximo;
    }

    // Reposiciona os dados dos membros já existentes para o novo local, logo após o novo header
    while (atual)
    {
        // Se o offset atual do membro não está na posição correta, move os dados
        if (atual->arquivo.offset != offsetDados)
        {
            moverDados(vc, atual->arquivo.offset, offsetDados, atual->arquivo.emDisco, maiorArquivo);
            atual->arquivo.offset = offsetDados;
        }

        // Atualiza o offset para o próximo membro
        offsetDados += atual->arquivo.emDisco;
        atual = atual->proximo;
    }

    // Processa cada novo arquivo passado por linha de comando
    for (int i = 3; i < argc; i++)
    {
        // Prepara um novo membro
        ArquivoMembro novoMembro;
        strncpy(novoMembro.nome, argv[i], sizeof(novoMembro.nome) - 1);
        novoMembro.nome[sizeof(novoMembro.nome) - 1] = '\0'; // garante que esteja terminado em '\0'
        novoMembro.uid = indice.ultimoUID + 1;                // gera novo UID
        indice.ultimoUID = novoMembro.uid;                    // atualiza o último UID usado
        novoMembro.modificacao = time(NULL);                  // marca a data/hora atual
        novoMembro.ordem = indice.lista.quantidade;           // ordem de inserção
        novoMembro.offset = offsetDados;                      // local onde os dados serão inseridos

        uint32_t tamanhoFinal = 0;

        // Insere o novo arquivo no .vc (com ou sem compressão, conforme o parâmetro)
        if (inserirArquivo(argv[i], vc, &offsetDados, &tamanhoFinal, compress) != 0)
        {
            fprintf(stderr, "Erro ao inserir os dados do arquivo %s no .vc\n", argv[i]);
            continue; // pula para o próximo arquivo se houver erro
        }

        // Atualiza os campos de tamanho do membro (original e em disco são iguais após inserção)
        novoMembro.tamanho = tamanhoFinal;
        novoMembro.emDisco = tamanhoFinal;

        // Adiciona o novo membro ao índice em memória
        adicionarAoIndice(&indice, novoMembro);
        printf("Arquivo '%s' adicionado ao índice e ao .vc com sucesso.\n", argv[i]);
    }

    // Volta para o início do arquivo .vc
    rewind(vc);

    // Salva o índice atualizado no arquivo
    if (salvarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
    }

    // Libera os recursos usados na memória
    destruirIndice(&indice);
    fclose(vc); // Fecha o arquivo .vc
}

// Função para inserir arquivos comprimidos no arquivador .vc
int vinac_insert_comp(int argc, char *argv[])
{
    // Verifica se foram passados argumentos suficientes
    if (argc < 4)
    {
        fprintf(stderr, "Uso: vinac -ic arquivo.vc arquivo1 [arquivo2 ...]\n");
        return (-1);
    }

    const char *arquivoVC = argv[2]; // Nome do arquivo .vc

    FILE *vc = NULL;
    IndiceArquivador indice;

    inicializarIndice(&indice); // Inicializa a estrutura do índice em memória

    // Verifica se o arquivo .vc já existe no disco
    if (access(arquivoVC, F_OK) == 0)
    {
        // Abre o arquivo existente em modo leitura e escrita binária
        vc = fopen(arquivoVC, "r+b");

        // Se falhar ao abrir, encerra com erro
        if (!vc)
        {
            fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return (-1);
        }

        // Carrega o índice do arquivo .vc para a estrutura em memória
        if (carregarIndice(vc, &indice) < 0)
        {
            fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
            fclose(vc);
            destruirIndice(&indice);
            return (-1);
        }
    }
    else
    {
        // Arquivo ainda não existe: cria novo .vc
        vc = fopen(arquivoVC, "w+b");

        // Se falhar ao criar o arquivo, encerra com erro
        if (!vc)
        {
            fprintf(stderr, "Erro ao criar o arquivo %s\n", arquivoVC);
            destruirIndice(&indice);
            return (-1);
        }

        // Inicializa o .vc com zero membros
        uint32_t numMembros = 0;

        // Verifica se conseguiu escrever o valor inicial
        if (fwrite(&numMembros, sizeof(uint32_t), 1, vc) != 1)
        {
            fprintf(stderr, "Erro ao inicializar o arquivo %s\n", arquivoVC);
            fclose(vc);
            destruirIndice(&indice);
            return (-1);
        }
    }

    // Calcula a quantidade final de arquivos (para saber o novo header)
    uint32_t quantidade_total = indice.lista.quantidade;

    // Conta apenas os arquivos que ainda não existem no índice
    for (int i = 3; i < argc; i++)
    {
        No *existente = buscarArquivo(&indice.lista, argv[i]);

        if (!existente)
        {
            quantidade_total++;
        }
    }

    // Calcula novo tamanho da área de diretório (header)
    uint64_t novoHeader = sizeof(uint32_t) + (quantidade_total * sizeof(ArquivoMembro));

    // Define o novo início da área de dados
    uint64_t offsetDados = novoHeader;

    // Determina o maior arquivo existente no índice (para definir o tamanho do buffer de movimentação)
    size_t maiorArquivo = 0;
    No *tmp = indice.lista.primeiro;

    while (tmp)
    {
        if (tmp->arquivo.tamanho > maiorArquivo)
        {
            maiorArquivo = tmp->arquivo.tamanho;
        }

        tmp = tmp->proximo;
    }

    // Move os dados dos arquivos já existentes para após o novo header
    No *atual = indice.lista.primeiro;

    while (atual)
    {
        // Se o offset atual do membro está fora do novo layout, move os dados
        if (atual->arquivo.offset != offsetDados)
        {
            moverDados(vc, atual->arquivo.offset, offsetDados, atual->arquivo.emDisco, maiorArquivo);
            atual->arquivo.offset = offsetDados;
        }

        offsetDados += atual->arquivo.emDisco;
        atual = atual->proximo;
    }

    // Processa cada arquivo informado na linha de comando
    for (int i = 3; i < argc; i++)
    {
        No *existente = buscarArquivo(&indice.lista, argv[i]);
        int diffTamanho = 0;
        int novoEmDisco = 0;
        uint32_t tamOriginal = 0;
        uint64_t offset = 0;

        if (existente)
        {
            // Caso o membro já exista, é uma substituição
            offset = existente->arquivo.offset;

            // Define o tamanho do buffer (mínimo 4096 bytes ou o tamanho do arquivo atual)
            size_t bufferSize = existente->arquivo.tamanho > 4096 ? existente->arquivo.tamanho : 4096;

            // Aloca buffer temporário
            unsigned char *buffer = malloc(bufferSize);

            // Verifica falha na alocação
            if (!buffer)
            {
                fprintf(stderr, "Erro de memória para buffer.\n");
                continue;
            }

            // Tenta inserir o novo conteúdo comprimido no mesmo local
            novoEmDisco = inserirComp(argv[i], vc, offset, existente->arquivo.emDisco, 1, buffer, bufferSize, &diffTamanho, &tamOriginal);
            free(buffer); // Libera o buffer após uso

            // Se falhou ao substituir
            if (novoEmDisco < 0)
            {
                fprintf(stderr, "Erro ao substituir o arquivo %s\n", argv[i]);
                continue;
            }

            // Se o tamanho mudou após compressão (mais ou menos bytes)
            if (diffTamanho != 0)
            {
                // Atualiza metadados do membro existente
                existente->arquivo.emDisco = novoEmDisco;
                existente->arquivo.tamanho = tamOriginal;
                existente->arquivo.modificacao = time(NULL);

                // Atualiza os offsets dos membros seguintes
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
                // Caso o conteúdo seja o mesmo tamanho, apenas atualiza os campos
                existente->arquivo.emDisco = novoEmDisco;
                existente->arquivo.tamanho = tamOriginal;
                existente->arquivo.modificacao = time(NULL);

                printf("Arquivo '%s' já está otimizado (compressão não reduz tamanho).\n", argv[i]);
            }
        }
        else
        {
            // Arquivo novo — será inserido no final do archive
            offset = offsetDados;

            // Tenta obter o tamanho real do arquivo para definir buffer adequado
            FILE *arq = fopen(argv[i], "rb");
            size_t bufferSize = maiorArquivo;

            if (arq)
            {
                fseek(arq, 0, SEEK_END);
                size_t tamArq = ftell(arq);

                if (tamArq > bufferSize)
                {
                    bufferSize = tamArq;
                }
                fclose(arq);
            }

            // Aloca buffer
            unsigned char *buffer = malloc(bufferSize);
            if (!buffer)
            {
                fprintf(stderr, "Erro de memória para buffer.\n");
                continue;
            }

            // Insere o novo arquivo com compressão
            novoEmDisco = inserirComp(argv[i], vc, offset, 0, 1, buffer, bufferSize, &diffTamanho, &tamOriginal);
            free(buffer);

            if (novoEmDisco < 0)
            {
                fprintf(stderr, "Erro ao inserir arquivo novo %s\n", argv[i]);
                continue;
            }

            // Preenche a estrutura do novo membro
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

            // Adiciona ao índice em memória
            adicionarAoIndice(&indice, novoMembro);

            printf("Arquivo '%s' inserido comprimido no .vc.\n", argv[i]);
            offsetDados += novoEmDisco; // Atualiza o próximo offset disponível
        }
    }

    // Salva o índice atualizado no início do .vc
    rewind(vc);

    if (salvarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
    }

    // Libera memória e fecha o arquivo
    destruirIndice(&indice);
    fclose(vc);

    return (0); // Sucesso
}

// Remove um arquivo do arquivador .vc
void vinac_remove(int argc, char *argv[])
{
    // Verifica se o número de argumentos está correto (vinac -r arquivo.vc membro)
    if (argc != 4)
    {
        fprintf(stderr, "Uso: vinac -r arquivo.vc arquivo_a_remover\n");
        return;
    }

    const char *arquivoVC = argv[2]; // Nome do arquivo .vc
    FILE *vc = fopen(arquivoVC, "r+b"); // Abre o arquivo para leitura e escrita binária

    // Verifica se o arquivo foi aberto com sucesso
    if (!vc)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", arquivoVC);
        return;
    }

    IndiceArquivador indice;
    inicializarIndice(&indice); // Inicializa a estrutura do índice em memória

    // Tenta carregar o índice do arquivo .vc
    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);                // Fecha o arquivo
        destruirIndice(&indice);  // Libera estrutura na memória
        return;
    }

    // Tenta remover o arquivo indicado pelo usuário
    if (removerArquivo(vc, &indice.lista, argv[3]))
    {
        printf("Arquivo '%s' removido com sucesso.\n", argv[3]);

        // Volta ao início do arquivo para salvar o novo índice
        rewind(vc);

        // Salva o índice atualizado após remoção
        if (salvarIndice(vc, &indice) < 0)
        {
            fprintf(stderr, "Erro ao salvar o índice no arquivador.\n");
        }
    }
    else
    {
        // Se o arquivo não foi encontrado no índice
        fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", argv[3]);
    }

    destruirIndice(&indice); // Libera memória usada pelo índice
    fclose(vc);              // Fecha o arquivo .vc
}

// Lista o conteúdo do arquivador .vc
void vinac_list(int argc, char *argv[])
{
    // Verifica se o número mínimo de argumentos foi fornecido
    if (argc < 3)
    {
        fprintf(stderr, "Uso: vinac -c arquivo.vc\n");
        return;
    }

    const char *arquivoVC = argv[2]; // Nome do arquivo .vc
    FILE *vc = fopen(arquivoVC, "rb"); // Abre o arquivo para leitura binária

    // Verifica se o arquivo existe e foi aberto corretamente
    if (!vc)
    {
        fprintf(stderr, "Erro: arquivo %s não existe\n", arquivoVC);
        return;
    }

    IndiceArquivador indice;
    inicializarIndice(&indice); // Inicializa o índice em memória

    // Tenta carregar o índice do arquivo .vc
    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc); // Fecha o arquivo caso haja falha
        return;
    }

    // Chama a função de listagem de arquivos no índice
    listarArquivos(&indice);

    destruirIndice(&indice); // Libera memória usada pela estrutura
    fclose(vc);              // Fecha o arquivo .vc
}

// Extrai arquivos do arquivador .vc
void vinac_extract(int argc, char *argv[])
{
    // Verifica se o número de argumentos é menor que 3, o que significa que a sintaxe do comando está incorreta.
    // Se for o caso, imprime a mensagem de uso e retorna.
    if (argc < 3)
    {
        fprintf(stderr, "Uso: vinac -x arquivo.vc [arquivos...]\n");
        return;
    }

    // O arquivo .vc especificado como argumento é atribuído à variável 'arquivoVC'.
    const char *arquivoVC = argv[2];

    // Tenta abrir o arquivo .vc para leitura binária. Se falhar, imprime mensagem de erro e retorna.
    FILE *vc = fopen(arquivoVC, "rb");

    if (!vc)
    {
        fprintf(stderr, "Erro: arquivo %s não existe\n", arquivoVC);
        return;
    }

    // Declara e inicializa o índice do arquivador, um tipo de estrutura que contém informações dos arquivos armazenados.
    IndiceArquivador indice;
    inicializarIndice(&indice);

    // Carrega o índice do arquivo .vc. Se falhar, imprime uma mensagem de erro, fecha o arquivo e retorna.
    if (carregarIndice(vc, &indice) < 0)
    {
        fprintf(stderr, "Erro ao carregar o índice do arquivador.\n");
        fclose(vc);
        return;
    }

    // Se não forem especificados arquivos adicionais para extração, todos os arquivos do índice serão extraídos.
    if (argc == 3)
    {
        No *atual = indice.lista.primeiro;

        // Itera sobre todos os arquivos no índice e tenta extraí-los um por um.
        while (atual)
        {
            // Tenta extrair o arquivo atual e imprime o sucesso ou erro.
            if (extrairArquivo(vc, atual->arquivo, ".") == 0)
            {
                printf("Arquivo '%s' extraído com sucesso.\n", atual->arquivo.nome);
            }
            else
            {
                fprintf(stderr, "Erro ao extrair o arquivo '%s'.\n", atual->arquivo.nome);
            }
            // Avança para o próximo arquivo no índice.
            atual = atual->proximo;
        }
    }
    // Se o argumento for "*", todos os arquivos serão extraídos.
    else if (argc == 4 && strcmp(argv[3], "*") == 0)
    {
        No *atual = indice.lista.primeiro;
        while (atual)
        {
            // Tenta extrair o arquivo atual e imprime o sucesso ou erro.
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
    // Se um único arquivo for especificado, tenta extrair esse arquivo.
    else if (argc == 4)
    {
        const char *nomeExtrair = argv[3];
        // Busca o arquivo especificado no índice.
        No *arquivo = buscarArquivo(&indice.lista, nomeExtrair);

        if (arquivo)
        {
            // Se o arquivo for encontrado, tenta extrair e imprime o sucesso ou erro.
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
            // Se o arquivo não for encontrado no índice, imprime a mensagem de erro.
            fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", nomeExtrair);
        }
    }
    else
    {
        // Para o caso de múltiplos arquivos serem especificados para extração, itera sobre eles.
        for (int i = 3; i < argc; i++)
        {
            const char *nomeExtrair = argv[i];
            // Busca cada arquivo no índice.
            No *arquivo = buscarArquivo(&indice.lista, nomeExtrair);

            if (arquivo)
            {
                // Se o arquivo for encontrado, tenta extrair e imprime o sucesso ou erro.
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
                // Se o arquivo não for encontrado, imprime a mensagem de erro.
                fprintf(stderr, "Arquivo '%s' não encontrado no arquivador.\n", nomeExtrair);
            }
        }
    }

    // Destrói o índice para liberar memória antes de fechar o arquivo .vc.
    destruirIndice(&indice);
    fclose(vc);  // Fecha o arquivo .vc.
}

// Move um arquivo dentro do arquivador .vc
void vinac_move(int argc, char *argv[])
{
    // Verifica se o número de argumentos fornecido é suficiente para realizar a operação de mover
    if (argc < 5)
    {
        fprintf(stderr, "Uso: vinac -m arquivo.vc posicao_atual nova_posicao\n");
        return;
    }

    // Tenta abrir o arquivo .vc especificado para leitura e escrita binária.
    FILE *vc = fopen(argv[2], "r+b");
    if (!vc)
    {
        perror("Erro ao abrir arquivo");
        return;
    }

    // Declara e inicializa o índice do arquivador.
    IndiceArquivador idx;
    inicializarIndice(&idx);

    // Carrega o índice do arquivo. Se falhar, imprime mensagem de erro e fecha o arquivo.
    if (carregarIndice(vc, &idx) < 0)
    {
        fprintf(stderr, "Erro ao carregar índice.\n");
        fclose(vc);
        return;
    }

    // Converte as posições de movimentação para números inteiros.
    uint32_t posAtual = atoi(argv[3]);
    uint32_t novaPos = atoi(argv[4]);

    // Verifica se as posições são válidas (não são iguais e estão dentro dos limites).
    if (posAtual == novaPos ||
        posAtual >= idx.lista.quantidade ||
        novaPos >= idx.lista.quantidade)
    {
        fprintf(stderr, "Posições inválidas.\n");
        destruirIndice(&idx);
        fclose(vc);
        return;
    }

    // Encontra o tamanho do maior arquivo no índice.
    size_t maior = encontrarMaiorArquivo(&idx.lista, 0);

    // Aloca buffers para armazenar dados temporariamente durante o processo de movimentação.
    unsigned char *buffer1 = malloc(maior);
    unsigned char *buffer2 = malloc(maior);

    // Verifica se a alocação de memória foi bem-sucedida.
    if (!buffer1 || !buffer2)
    {
        perror("malloc");
        free(buffer1);
        free(buffer2);
        destruirIndice(&idx);
        fclose(vc);
        return;
    }

    // Localiza o nó do arquivo a ser movido, baseado na posição atual.
    No *noAtual = idx.lista.primeiro;
    for (uint32_t i = 0; i < posAtual; i++)
    {
        noAtual = noAtual->proximo;
    }

    // Armazena o tamanho do arquivo a ser movido.
    size_t tamMovido = noAtual->arquivo.emDisco;

    // Define o ponteiro de leitura do arquivo para o início do arquivo a ser movido.
    fseek(vc, noAtual->arquivo.offset, SEEK_SET);
    // Lê os dados do arquivo a ser movido para o buffer1.
    fread(buffer1, 1, tamMovido, vc);

    // Se a posição atual for menor que a nova posição, os arquivos entre as duas posições são movidos para frente.
    if (posAtual < novaPos)
    {
        No *tmp = noAtual->proximo;

        // Move os arquivos que precisam ser "empurrados" para frente para abrir espaço.
        while (tmp && tmp->arquivo.ordem <= novaPos)
        {
            fseek(vc, tmp->arquivo.offset, SEEK_SET);
            fread(buffer2, 1, tmp->arquivo.emDisco, vc);

            fseek(vc, tmp->arquivo.offset - tamMovido, SEEK_SET);
            fwrite(buffer2, 1, tmp->arquivo.emDisco, vc);
            tmp->arquivo.offset -= tamMovido;

            tmp = tmp->proximo;
        }

        // Atualiza o offset do arquivo que está sendo movido.
        noAtual->arquivo.offset = tmp ? tmp->arquivo.offset : (uint64_t)ftell(vc);
        noAtual->arquivo.offset -= tamMovido;
    }
    else
    {
        // Caso a posição atual seja maior que a nova posição, os arquivos entre as duas posições são movidos para trás.
        No *tmp = noAtual->anterior;

        // Move os arquivos que precisam ser "empurrados" para trás.
        while (tmp && tmp->arquivo.ordem >= novaPos)
        {
            fseek(vc, tmp->arquivo.offset, SEEK_SET);
            fread(buffer2, 1, tmp->arquivo.emDisco, vc);

            fseek(vc, tmp->arquivo.offset + tamMovido, SEEK_SET);
            fwrite(buffer2, 1, tmp->arquivo.emDisco, vc);
            tmp->arquivo.offset += tamMovido;

            tmp = tmp->anterior;
        }

        // Atualiza o offset do arquivo que está sendo movido.
        if (tmp)
        {
            noAtual->arquivo.offset = tmp->arquivo.offset + tmp->arquivo.emDisco;
        }
        else
        {
            // Caso o arquivo movido seja o primeiro, define o offset com base no início do arquivo.
            noAtual->arquivo.offset = sizeof(uint32_t) + idx.lista.quantidade * sizeof(ArquivoMembro);
        }
    }

    // Reposiciona o ponteiro do arquivo para o novo offset do arquivo a ser movido e escreve seus dados.
    fseek(vc, noAtual->arquivo.offset, SEEK_SET);
    fwrite(buffer1, 1, tamMovido, vc);

    // Libera a memória alocada para os buffers.
    free(buffer1);
    free(buffer2);

    // Atualiza a lista do índice movendo o nó na lista.
    moverNoLista(&idx.lista, posAtual, novaPos);
    // Atualiza as ordens no índice para refletir a mudança.
    atualizarOrdens(&idx.lista);

    // Reposiciona o ponteiro do arquivo no início para garantir que a alteração seja salva corretamente.
    rewind(vc);

    // Salva o índice atualizado no arquivo .vc. Se falhar, imprime mensagem de erro.
    if (salvarIndice(vc, &idx) < 0)
    {
        fprintf(stderr, "Erro ao salvar índice atualizado.\n");
    }

    // Destrói o índice e fecha o arquivo .vc.
    destruirIndice(&idx);
    fclose(vc);

    // Informa ao usuário que o arquivo foi movido corretamente.
    printf("Arquivo movido corretamente.\n");
}