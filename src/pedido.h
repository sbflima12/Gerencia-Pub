#ifndef PEDIDO_H
#define PEDIDO_H
#include "estoque.h"

typedef struct{
    char nome[31];
    int quantidade;
    float precoUnitario;
    float precoTotal;
} Produto;

typedef struct{
    int id;
    char cpf[32];
    char data[20];
    int quantidadeProdutos;
    float valorTotal;
    Produto *produto;
} Pedido;

FILE* abrirArquivosPedidos(int modo);

void registrarPedido();
void gerarArquivoPedidos(int id, const char *cpf, int qtdProdutos, float total);
void gerarArquivoItensVendidos(Pedido *pedido);
void processarPagamento(float valorTotal, float *pagoCliente, float *troco);

int gerarProximoIDPedido();
int verificarProdutosPedido(Pedido *pedido);
int atualizarEstoquePedido(Pedido *pedido);

#endif
