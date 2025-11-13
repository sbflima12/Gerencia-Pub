#ifndef UTILS_H
#define UTILS_H
#include "pedido.h"
#include "estoque.h"

typedef struct {
    char nome[50];
    char cpf[20];
    char telefone[15];
    char email[100];
}Cliente;

typedef struct {
    char nome[50];
    int quantidadeTotal;
    float valorTotal;
} ResumoProduto;

FILE* abrirArquivoClientes(int modo);

void menuInicial();
void sair();
void cadastrarCliente();
void consultarCliente();
void lerDadosRelatorio(Pedido **pedidos, size_t *numPedidos, Produto **itens, int **idsItens, size_t *numItens, int filtrar, const char *dataFiltro);
void gerarArquivoRelatorio(const char *dataFiltro, int filtrar, ResumoProduto *resumo, size_t numResumo, float totalVendas, size_t numPedidos);
void gerarRelatorio();

int verificarCliente(Cliente c);

float calcularValorGasto(const char* buscaCpf);

#endif
