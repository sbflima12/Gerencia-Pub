#include <stdio.h>
#include <string.h>
#include <stdlib.h>     
#include <time.h>  
#include "utils.h"
#include "estoque.h"
#include "pedido.h"

typedef struct {
    char nome[50];
    char cpf[20];
    char telefone[15];
    char email[100];
}Cliente;

FILE* abrirArquivoClientes(int modo) {
    FILE *arquivo = NULL;

    switch (modo) {
        case 1: arquivo = fopen("clientes.txt", "r"); break;
        case 2: arquivo = fopen("clientes.txt", "a"); break;
        default:
            printf("Modo de abertura inválido para clientes.\n");
            return NULL;
    }

    if (arquivo == NULL)
        printf("Erro ao abrir o arquivo de clientes.\n");

    return arquivo;
}

void cadastrarCliente() {
    
    Cliente c;
    
    printf("\n--------Cadastro de Cliente--------\n");
    
    printf("Digite o nome completo:");
    scanf(" %[^\n]", c.nome);
    
    printf("Digite o CPF:");
    scanf("%s", c.cpf);
    
    printf("Digite o telefone:");
    scanf(" %s", c.telefone);
    
    printf("Digite o email:");
    scanf("%s", c.email);
    
    FILE *arqCliente = abrirArquivoClientes(2);
    if (arqCliente == NULL){
        printf("Erro ao abrir o arquivo de clientes!");
        return;
    }
    
    fprintf(arqCliente, "%s;%s;%s;%s\n", c.nome, c.cpf, c.telefone, c.email);
    
    fclose(arqCliente);
    
    printf("\nCliente cadastrado com sucesso!\n");
}

float calcularValorGasto(const char* buscaCpf) {
    
    FILE *arqPedido = abrirArquivosPedidos(1);
    if (arqPedido == NULL) {
        printf("\nErro ao abrir o arquivo pedidos.txt para identificar o valor total gasto!\n");
        return 0.0;
    }

    char cpfPedido[20];
    float valorGasto; 
    float valorTotalGasto = 0.0; 



    while(fscanf(arqPedido, "%d;%[^;];%[^;];%*d;%f\n", cpfPedido, &valorGasto) != EOF) {
        if(strcmp(buscaCpf, cpfPedido) == 0) {
            valorTotalGasto += valorGasto;
        }
    }

    fclose(arqPedido);
    
    return valorTotalGasto;
}

void consultarCliente() {
    
    Cliente c; 
    char senha[7];
    const char senhaCorreta[] = "543210";
    char buscaCpf[20];
    int encontrado = 0;
    
    printf("\n---- Acessar Dados ----\n");
    printf("Digite a senha:");
    scanf("%s", senha);
    
    if (strcmp(senha,senhaCorreta) != 0) {
        printf("Senha incorreta! Acesso negado.\n");
        return;
    }
        
    printf("Digite o CPF do cliente:");
    scanf("%s", buscaCpf);

    FILE *arqCliente = fopen("clientes.txt", "r");
    if (arqCliente == NULL){
        printf("Erro ao abrir o arquivo de clientes!\n");
        return;
    }
    
    while(fscanf(arqCliente, "%[^;];%[^;];%[^;];%[^\n]\n", c.nome, c.cpf, c.telefone, c.email) != EOF) {
        if (strcmp(buscaCpf, c.cpf) == 0) {
            encontrado = 1;
            printf("\n---- Dados do Cliente ----\n");
            printf(" Nome:%s\n CPF:%s\n Telefone:%s\n Email:%s", c.nome, c.cpf, c.telefone, c.email);
            
            float totalGasto = calcularValorGasto(buscaCpf);
            
            printf("\n Valor total gasto: R$ %.2f\n", totalGasto);
            break;  
        }
    } 
    
    fclose(arqCliente);
    
    if(!encontrado) {
        printf("\nCliente com CPF %s não encontrado.\n", buscaCpf);
        return;
    }
}

//lê os arquivos e retorna dados
void lerDadosRelatorio(Pedido **pedidos, size_t *numPedidos, Produto **itens, int **idsItens, size_t *numItens, int filtrar, const char *dataFiltro) {
    FILE *fpPedidos = abrirArquivosPedidos(1);   
    FILE *fpItens = abrirArquivosPedidos(4);     

    if (!fpPedidos || !fpItens) {
        printf("\nErro: não foi possível abrir os arquivos pedidos.txt ou itens_vendidos.txt.\n");
        if (fpPedidos){
            fclose(fpPedidos);
        }
        if (fpItens){
            fclose(fpItens);
        }
        return;
    }

    char linha[256];
    *pedidos = NULL;
    *numPedidos = 0;
    size_t capPedidos = 0;

    //leitura de pedidos.txt
    while (fgets(linha, sizeof(linha), fpPedidos)) {
        Pedido p;
        if (sscanf(linha, "%d;%[^;];%[^;];%d;%f", &p.id, p.cpf, p.data, &p.quantidadeProdutos, &p.valorTotal) == 5) {
            if (!filtrar || strcmp(p.data, dataFiltro) == 0) {
                if (*numPedidos == capPedidos) {
                    capPedidos = (capPedidos == 0) ? 10 : capPedidos * 2;
                    *pedidos = realloc(*pedidos, capPedidos * sizeof(Pedido));
                }
                (*pedidos)[(*numPedidos)++] = p;
            }
        }
    }
    fclose(fpPedidos);

    //leitura de itens_vendidos.txt
    *itens = NULL;
    *idsItens = NULL;
    *numItens = 0;
    size_t capItens = 0;

    while (fgets(linha, sizeof(linha), fpItens)) {
        int idPedido;
        Produto it;

        if (sscanf(linha, "%d;%[^;];%d;%f;%f", &idPedido, it.nome, &it.quantidade, &it.precoUnitario, &it.precoTotal) == 5) {
            int incluir = 0;
            if (!filtrar) incluir = 1;
            else {
                for (size_t i = 0; i < *numPedidos; i++) {
                    if ((*pedidos)[i].id == idPedido) {
                        incluir = 1;
                        break;
                    }
                }
            }

            if (incluir) {
                if (*numItens == capItens) {
                    capItens = (capItens == 0) ? 20 : capItens * 2;
                    *itens = realloc(*itens, capItens * sizeof(Produto));
                    *idsItens = realloc(*idsItens, capItens * sizeof(int));
                }
                (*itens)[*numItens] = it;
                (*idsItens)[*numItens] = idPedido;
                (*numItens)++;
            }
        }
    }
    fclose(fpItens);
}

void gerarArquivoRelatorio(const char *dataFiltro, int filtrar, ResumoProduto *resumo, size_t numResumo, float totalVendas, size_t numPedidos){
    FILE *arqRelatorio = fopen("relatorio_resumido.txt", "w");
    if (!arqRelatorio) {
        printf("Erro ao criar relatorio_resumido.txt!\n");
    } else {
        fprintf(arqRelatorio, "=========== RELATÓRIO DE VENDAS ===========\n");
        if (filtrar){
            fprintf(arqRelatorio, "Data filtrada: %s\n", dataFiltro);
        }

        fprintf(arqRelatorio, "Pedidos processados: %zu\n", numPedidos);
        fprintf(arqRelatorio, "--------------------------------------------\n");

        for (size_t i = 0; i < numResumo; i++) {
            fprintf(arqRelatorio, "%s;%d;%.2f\n", resumo[i].nome, resumo[i].quantidadeTotal, resumo[i].valorTotal);
        }

        fprintf(arqRelatorio, "--------------------------------------------\n");
        fprintf(arqRelatorio, "TOTAL GERAL DE VENDAS: R$ %.2f\n", totalVendas);
        fprintf(arqRelatorio, "TICKET MÉDIO POR PEDIDO: R$ %.2f\n", totalVendas / numPedidos);
        fclose(arqRelatorio);

        printf("\nRelatório salvo em 'relatorio_resumido.txt'.\n");
    }
}

void gerarRelatorio() {
    int filtrar;
    char dataFiltro[20] = "";

    printf("\nDeseja filtrar por data? (1 = sim, 0 = não): ");
    scanf("%d", &filtrar);
    if (filtrar) {
        printf("Digite a data (dd/mm/aaaa): ");
        scanf("%s", dataFiltro);
    }

    Pedido *pedidos = NULL;
    Produto *itens = NULL;
    int *idsItens = NULL;
    size_t numPedidos = 0, numItens = 0;

    lerDadosRelatorio(&pedidos, &numPedidos, &itens, &idsItens, &numItens, filtrar, dataFiltro);

    if (numPedidos == 0) {
        printf("\nNenhum pedido encontrado para o filtro aplicado.\n");
        free(pedidos);
        free(itens);
        free(idsItens);
        return;
    }

    //Consolidação de vendas
    ResumoProduto *resumo = NULL;
    size_t numResumo = 0, capResumo = 0;
    float totalVendas = 0.0f;

    for (size_t i = 0; i < numItens; i++) {
        totalVendas += itens[i].precoTotal;

        int encontrado = 0;
        for (size_t j = 0; j < numResumo; j++) {
            if (strcmp(resumo[j].nome, itens[i].nome) == 0) {
                resumo[j].quantidadeTotal += itens[i].quantidade;
                resumo[j].valorTotal += itens[i].precoTotal;
                encontrado = 1;
                break;
            }
        }

        if (!encontrado) {
            if (numResumo == capResumo) {
                capResumo = (capResumo == 0) ? 10 : capResumo * 2;
                resumo = realloc(resumo, capResumo * sizeof(ResumoProduto));
            }
            strcpy(resumo[numResumo].nome, itens[i].nome);
            resumo[numResumo].quantidadeTotal = itens[i].quantidade;
            resumo[numResumo].valorTotal = itens[i].precoTotal;
            numResumo++;
        }
    }

    //exibição do relatório
    printf("\n=========== RELATÓRIO RESUMIDO DE VENDAS ===========\n");
    if (filtrar) printf("Data filtrada: %s\n", dataFiltro);
    printf("Pedidos processados: %zu\n", numPedidos);
    printf("--------------------------------------------\n");

    printf("%-25s | %-10s | %-10s\n", "PRODUTO", "QTD", "TOTAL (R$)");
    printf("--------------------------------------------\n");
    for (size_t i = 0; i < numResumo; i++) {
        printf("%-25s | %-10d | %-10.2f\n", resumo[i].nome, resumo[i].quantidadeTotal, resumo[i].valorTotal);
    }
    printf("--------------------------------------------\n");
    printf("TOTAL GERAL DE VENDAS: R$ %.2f\n", totalVendas);
    printf("TICKET MÉDIO POR PEDIDO: R$ %.2f\n", totalVendas / numPedidos);

    //gerar arquivo
    char opcao;
    printf("\nDeseja gerar arquivo de relatório resumido? (s/n): ");
    scanf(" %c", &opcao);

    if (opcao == 's' || opcao == 'S') {
        gerarArquivoRelatorio(dataFiltro, filtrar, resumo, numResumo, totalVendas, numPedidos);
    } 

    free(pedidos);
    free(itens);
    free(idsItens);
    free(resumo);
}

void sair(){
    printf("Saindo...");
}

void menuInicial(){
    int resposta = -1;
    void (*gerenciar[])() = {sair, menuCadastroProduto, menuConsultarProdutos, registrarPedido, cadastrarCliente, consultarCliente, gerarRelatorio};
    
    while (resposta!=0){
        printf("--------------MENU INICIAL--------------");
        printf("\n O que deseja fazer?");
        printf("\n(1) Cadastrar produto\n(2) Consultar produto\n(3) Registrar pedidos\n(4) Cadastrar cliente\n(5) Consultar cliente\n(6) Visualizar Relatório de Vendas \n(0) Sair");
        printf("\nSelecionar: ");
        scanf("%d",&resposta);
        printf("\n----------------------------------------");

        if (resposta >= 0 && resposta <= 6) {
            (*gerenciar[resposta])(); 
        } else {
            printf("Opção inválida!\n");
        }  
    }
}
