#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "utils.h"
#include "estoque.h"

#define FORMATO_ESCRITA "%06d %c %-20.20s %010.2f %06d %1d\n"
#define FORMATO_LEITURA "%6d %c %20c %10f %6d %1d\n"

//alterar
FILE* abrirArquivoEstoque(int modo) {
    FILE* arquivo = NULL;
    switch (modo) {
        case 1:
            arquivo = fopen("estoque.txt", "a");
            break;
        case 2:
            arquivo = fopen("estoque.txt", "r");
            if (arquivo == NULL) {
                arquivo = fopen("estoque.txt", "w+"); 
                if (arquivo != NULL){
                    fclose(arquivo);
                }
                arquivo = fopen("estoque.txt", "r");
            }
            break;
        case 3:
            arquivo = fopen("estoque.txt", "r+");
            if (arquivo == NULL) {
                arquivo = fopen("estoque.txt", "w+");
                if (arquivo != NULL){
                    fclose(arquivo);
                }
                arquivo = fopen("estoque.txt", "r+");
            }
            break;
        case 4:
            arquivo = fopen("estoque.txt", "w");
            break;
        default:
            printf("Modo inválido.\n");
            return NULL;
    }

    if (arquivo == NULL)
        printf("Erro ao abrir o arquivo.\n");

    return arquivo;
}

int verificarProduto(int codigo) {
    Produtos produtoLido;
    int encontrado = 0;

    FILE *arquivo = abrirArquivoEstoque(2);
    if (arquivo == NULL){
        return 0;
    }

    while(fscanf(arquivo, FORMATO_LEITURA, &produtoLido.codigo, &produtoLido.tipo, produtoLido.nome, &produtoLido.preco, &produtoLido.quantidade, &produtoLido.status)==6){
        produtoLido.nome[20] = '\0';
        if (produtoLido.codigo==codigo) {
            encontrado=1;
            break;  
        }
    }

    fclose (arquivo);
    return encontrado;
}

void removerEspacoFinal(char *nome) {//Remove espaços em branco do final da string
    if (nome==NULL){ //Verifica se o ponteiro está vazio
        return;
    }
    
    size_t tamanho = strlen(nome);
    if(tamanho == 0){
        return;
    }

    char *ponteiroScanner=nome+tamanho-1; //Aponta para o último caractere da string
    
    //Encontra o último caractere útil
    while ((ponteiroScanner>=nome)&&(isspace((unsigned char)*ponteiroScanner))) {
        *ponteiroScanner = '\0';
        ponteiroScanner--;
    }   
}

//CADASTRO DE PRODUTOS----------------------------------------------------------------------------------------------
void cadastrarProduto(){
    Produtos produto;

    printf("\n-------Cadastro de Produtos-------\n");
    printf("Digite o código do produto (até 6 números): ");
    scanf("%d", &produto.codigo);

    //Verifica se existe um produto com esse código no estoque
    int encontrado = verificarProduto(produto.codigo);
    if(encontrado == 1){
        printf("Já existe um produto com o código %d.\n", produto.codigo);
        return;
    }

    //Se não existe, continua normalmente
    printf("Digite o tipo de produto ('C' para comidas, 'B' para bebidas): ");
    scanf(" %c", &produto.tipo);
    printf("Digite o nome do produto (máx 20 caracteres): ");
    scanf(" %20[^\n]", produto.nome);
    printf("Digite o preço do produto: ");
    scanf(" %f", &produto.preco);
    printf("Digite a quantidade do produto: ");
    scanf(" %d", &produto.quantidade);
    produto.status=1; // Novo produto é sempre ATIVO

    FILE *arquivo = abrirArquivoEstoque(1); //("estoque.txt", "a")
    if(arquivo==NULL){
        return;
    }
    fprintf(arquivo, FORMATO_ESCRITA, produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
    fclose(arquivo);

    printf("Produto cadastrado com sucesso.\n");

}

//ALTERAÇÃO DE PRODUTOS----------------------------------------------------------------------------------------------
void menuAlterarProduto(Produtos *produto) {
    int opcao=0;
    do{
        printf("\n\n--- Produto Encontrado ---\n");
        printf("Tipo: %c\nNome: %s\nPreço: R$%.2f\nQuantidade: %d\n", produto->tipo, produto->nome, produto->preco, produto->quantidade);
        printf("--------------------------\n");
        printf("O que deseja alterar?\n");
        printf("(1) Nome\n(2) Preço\n(3) Tipo de produto\n(4) Quantidade\n(0) Salvar alterações e Voltar ao menu\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch(opcao){
            case 1:
                printf("Digite o novo nome (máx 20 caracteres): ");
                scanf(" %20[^\n]", produto->nome);
                break;
            case 2:
                printf("Digite o novo preço: ");
                scanf(" %f", &produto->preco); 
                break;
            case 3:
                printf("Digite o novo tipo de produto: ");
                scanf(" %c", &produto->tipo);
                break;
            case 4:
                printf("Digite a nova quantidade do produto: ");
                scanf(" %d", &produto->quantidade);
                break;
            case 0:
                printf("Alterações salvas.\n");
                break;
            default:
                printf("Opção inválida\n");
                break;
        }
    }while(opcao!=0);
}

void alterarProduto(){
    int codigoAlterar, encontrado=0;
    Produtos produto;
    long int posicaoLinha; 

    FILE *arquivo=abrirArquivoEstoque(3); //"estoque.txt", "r+"
    if(arquivo==NULL){ 
        return;
    }
    
    printf("\n-------------------Alterar Produto-------------------\n");
    printf("Digite o código do produto que deseja alterar: ");
    scanf("%d", &codigoAlterar);
    
    while(1){
        posicaoLinha=ftell(arquivo); 

        if(fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status)!=6) {
            break;
        }
        
        produto.nome[20]='\0'; 

        if((produto.codigo==codigoAlterar)&&(produto.status==1)){
            encontrado=1;
            removerEspacoFinal(produto.nome); 
            menuAlterarProduto(&produto); 
            fseek(arquivo, posicaoLinha, SEEK_SET);//Posiciona o cursor no inicio da linha que acabou de ler 
            fprintf(arquivo, FORMATO_ESCRITA, produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
            
            break; 
        }
    }

    fclose(arquivo);
    
    if(encontrado){
        printf("\nProduto alterado com sucesso\n");
    }else{
        printf("\nProduto com código %d não encontrado ou inativo.\n", codigoAlterar);
    }
}

//EXCLUSÃO DE PRODUTOS ----------------------------------------------------------------------------
void excluirProduto(){
    int codigoExcluir, encontrado=0;
    Produtos produto;
    long int posicaoLinha;
    char opcao;

    FILE *arquivo=abrirArquivoEstoque(3); //"estoque.txt", "r+"
    if(arquivo==NULL){ 
        return;
    }

    printf("\n-------------------Excluir Produto-------------------\n");
    printf("Digite o código do produto que deseja excluir: ");
    scanf("%d", &codigoExcluir);

    while(1){
        posicaoLinha=ftell(arquivo);

        if(fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) != 6) {
            break; // Verifica o fim do arquivo
        }

        produto.nome[20]='\0';

        if((produto.codigo==codigoExcluir)&&(produto.status==1)){
            encontrado=1;
            removerEspacoFinal(produto.nome);
            
            printf("\nProduto Encontrado:\n");
            printf("Código: %d\nTipo: %c\nNome: %s\nPreço: R$%.2f\nQuantidade: %d\nStatus: %d\n", produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
            printf("Tem certeza que deseja marcar como INATIVO? (s/n): ");
            scanf(" %c", &opcao);

            if((opcao=='s')||(opcao=='S')){
                produto.status=0; // Exclusão lógica
                fseek(arquivo, posicaoLinha, SEEK_SET); 
                fprintf(arquivo, FORMATO_ESCRITA, produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
                printf("\nProduto '%s' marcado como inativo.\n", produto.nome);
            }else if((opcao=='n')||(opcao=='N')){
                printf("\nOperação cancelada.\n");
            }else{
                printf("Opção inválida\n");
            }
            break;
        }
    }
    
    fclose(arquivo);

    if(encontrado==0){
        printf("\nProduto com código %d não encontrado ou já está inativo.\n", codigoExcluir);
    }
}

//ATIVAÇÃO DE PRODUTOS----------------------------------------------------------------------------
void ativarProduto(){
    int codigoAtivar, encontrado=0;
    Produtos produto;
    long int posicaoLinha;
    char opcao;

    FILE *arquivo = abrirArquivoEstoque(3); //"estoque.txt", "r+"
    if(arquivo == NULL){ 
        return;
    }

    printf("\n-------------------Ativar Produto-------------------\n");
    printf("Digite o código do produto que deseja REATIVAR: ");
    scanf("%d", &codigoAtivar);

    while(1){
        posicaoLinha=ftell(arquivo);
        if(fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status)!=6){
            break; //Encerra o loop se chegar no fim do arquivo
        }

        produto.nome[20]='\0';
        
        if((produto.codigo==codigoAtivar)&&(produto.status==0)){
            encontrado = 1;
            removerEspacoFinal(produto.nome);
            
            printf("\nProduto Encontrado (INATIVO):\n");
            printf("Código: %d\nTipo: %c\nNome: %s\nPreço: R$%.2f\nQuantidade: %d\nStatus: %d\n", produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
            printf("Tem certeza que deseja marcar como ATIVO? (s/n): ");
            scanf(" %c", &opcao);

            if((opcao=='s')||(opcao=='S')){
                produto.status=1; // Ativação lógica
                fseek(arquivo, posicaoLinha, SEEK_SET); 
                fprintf(arquivo, FORMATO_ESCRITA, produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
                printf("\nProduto '%s' marcado como ativo.\n", produto.nome);
            
            }else if((opcao=='n')||(opcao=='N')){
                printf("\nOperação cancelada.\n");
            }else{
                printf("Opção inválida\n");
            }
            break;
        }
    }
    
    fclose(arquivo);

    if(encontrado==0){
        printf("\nProduto com código %d não encontrado ou já está ativo.\n", codigoAtivar);
    }
}

//LISTA DE PRODUTOS---------------------------------------------------------------------------------------------------
void listarTodos(){
    FILE* arquivo = abrirArquivoEstoque(2);
    if(arquivo == NULL){
        return;
    }

    Produtos produto;
    
    printf("\n------ Lista de Produtos ------\n");
    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade,  &produto.status) == 6) {
        if(produto.status==1){
        produto.nome[20]='\0';
        removerEspacoFinal(produto.nome);
        printf("Código:%06d | Tipo:%c | Nome:%s | R$%.2f | Qtd:%d | Status: %d\n", produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade, produto.status);
        }
    }
    
    fclose(arquivo);
}

//listar apenas as bebidas do estoque
void listarBebidas(){
    FILE* arquivo = abrirArquivoEstoque(2);
    if(arquivo == NULL){
        return;
    }

    Produtos produto;

    printf("\n------ Lista de Bebidas ------\n");
    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade,  &produto.status) == 6) {
        if ((produto.tipo == 'B' || produto.tipo == 'b')&&(produto.status==1)){
            produto.nome[20] = '\0'; 
            removerEspacoFinal(produto.nome);
            printf("%06d | %s | R$%.2f | %d un\n", produto.codigo, produto.nome, produto.preco, produto.quantidade);
        }   
    }

    fclose(arquivo);
}

//listar todas as comidas do estoque
void listarComidas(){
    FILE *arquivo = abrirArquivoEstoque(2);
    if (arquivo == NULL){
        return;
    } 

    Produtos produto;
    printf("\n------ Lista de Comidas ------\n");
    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) == 6) {
        if ((produto.tipo == 'C' || produto.tipo == 'c')&&(produto.status == 1)){
            produto.nome[20] = '\0';
            removerEspacoFinal(produto.nome);
            printf("%06d | %s | R$%.2f | %d un\n", produto.codigo, produto.nome, produto.preco, produto.quantidade);
        }
        
    }
    fclose(arquivo);
}

void consultarProdutoPorCodigo(){
    FILE *arquivo = abrirArquivoEstoque(2);
    if (arquivo == NULL){
        return;
    }

    Produtos produto;
    int codigoProcurado, encontrado = 0;
    printf("\nDigite o código do produto: ");
    scanf("%d", &codigoProcurado);

    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) == 6) {
        if (produto.codigo == codigoProcurado) {
            produto.nome[20] = '\0';
            removerEspacoFinal(produto.nome);
            printf("\n--- Produto Encontrado ---\n");
            printf("Código:%06d | Tipo:%c | Nome:%s | R$%.2f | Qtd:%d\n", produto.codigo, produto.tipo, produto.nome, produto.preco, produto.quantidade);
            encontrado = 1;
            break;
        }
    }

    if (!encontrado)
        printf("Produto não encontrado.\n");

    fclose(arquivo);
}

//FUNÇÕES AUXILIARES PARA PEDIDOS---------------------------------------------------------------------------------------------------
int obterPrecoQuantidadePorNome(const char nomeProduto[], float *precoUnitario, int *quantidadeDisponivel) {
    FILE *arquivo = abrirArquivoEstoque(2);
    if (!arquivo) return 0;

    Produtos produto;

    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) == 6) {
        produto.nome[20]='\0';
        removerEspacoFinal(produto.nome);
        if ((strcmp(produto.nome, nomeProduto) == 0) && (produto.status==1)) {
            *precoUnitario = produto.preco;
            *quantidadeDisponivel = produto.quantidade;
            fclose(arquivo);
            return 1;
        }
    }
    fclose(arquivo);
    return 0;
}

int obterPrecoQuantidadePorCodigo(int codigoBusca, float *precoUnitario, int *quantidadeDisponivel, char *nomeProduto) {
    FILE *arquivo = abrirArquivoEstoque(2);
    if (!arquivo) return 0;

    Produtos produto;

    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) == 6) {
        if ((produto.codigo == codigoBusca) &&(produto.status==1)) {
            *precoUnitario = produto.preco;
            *quantidadeDisponivel = produto.quantidade;
            produto.nome[20]='\0';
            removerEspacoFinal(produto.nome);
            strcpy(nomeProduto, produto.nome);
            fclose(arquivo);
            return 1;
        }
    }

    fclose(arquivo);
    return 0;
}

//ATUALIZAR ESTOQUE---------------------------------------------------------------------------------------------------
int atualizarEstoque(char nomeProduto[], int quantidadeAlterar, int modo) {
    // modo = 1 -> venda (subtrai)
    // modo = 2 -> reposição (soma)

    FILE *arquivo = abrirArquivoEstoque(2); // modo leitura
    if (arquivo == NULL) {
        printf("estoque.txt não encontrado.\n");
        return 0;
    }

    Produtos *lista = NULL;
    size_t total = 0, capacidade = 0;
    int encontrado = 0;
    int indiceEncontrado = -1;

    // Lê todos os produtos do arquivo e armazena em memória
    Produtos produto;
    while (fscanf(arquivo, FORMATO_LEITURA, &produto.codigo, &produto.tipo, produto.nome, &produto.preco, &produto.quantidade, &produto.status) == 6) {
        produto.nome[20]='\0';
        if (total == capacidade) {
            capacidade = (capacidade == 0) ? 10 : capacidade * 2;
            lista = realloc(lista, capacidade * sizeof(Produtos));
            if (!lista) {
                printf("Erro de memória.\n");
                fclose(arquivo);
                return 0;
            }
        }

        lista[total] = produto;
        // verifica se é o produto que deve ser atualizado
        removerEspacoFinal(produto.nome);
        if (strcmp(produto.nome, nomeProduto) == 0) {
            encontrado = 1;
            indiceEncontrado = (int)total;
        }

        total++;
    }
    fclose(arquivo);

    if (!encontrado) {
        printf("Produto '%s' não encontrado no estoque.\n", nomeProduto);
        free(lista);
        return 0;
    }

    // Atualiza o produto encontrado
    Produtos *prod = &lista[indiceEncontrado];

    if (modo == 1) { // venda (subtrai)
        if (prod->quantidade < quantidadeAlterar) {
            printf("Estoque insuficiente de '%s'. Quantidade disponível: %d\n",
                   prod->nome, prod->quantidade);
            free(lista);
            return -1;
        }
        prod->quantidade -= quantidadeAlterar;
        printf("Produto '%s' atualizado: nova quantidade %d\n",
               prod->nome, prod->quantidade);
    } 
    else if (modo == 2) { // reposição (soma)
        prod->quantidade += quantidadeAlterar;
        printf("Produto '%s' reabastecido: nova quantidade %d\n",
               prod->nome, prod->quantidade);
    } 
    else {
        printf("Modo inválido.\n");
        free(lista);
        return 0;
    }

 
    arquivo = abrirArquivoEstoque(4);
    if (arquivo == NULL) {
        printf("Erro ao reabrir estoque.txt para escrita.\n");
        free(lista);
        return 0;
    }

    for (size_t i = 0; i < total; i++) {
        fprintf(arquivo, FORMATO_ESCRITA, lista[i].codigo, lista[i].tipo, lista[i].nome, lista[i].preco, lista[i].quantidade, lista[i].status);
    }

    fclose(arquivo);
    free(lista);
    return 1;
}

//MENUS---------------------------------------------------------------------------------------------------
void menuConsultarProdutos() {
    int opcao;
    do {
        printf("\n--- MENU CONSULTA PRODUTOS ---\n");
        printf("1. Listar todos\n2. Listar bebidas\n3. Listar comidas\n4. Consultar por código\n0. Voltar\nEscolha: ");
        scanf("%d", &opcao);

        switch (opcao) {
            case 1: listarTodos(); break;
            case 2: listarBebidas(); break;
            case 3: listarComidas(); break;
            case 4: consultarProdutoPorCodigo(); break;
            case 0: break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 0);
}

void menuCadastroProduto() {
    int opcao=-1;
    void (*gerenciar[])()={cadastrarProduto, alterarProduto, excluirProduto, ativarProduto};
    
    do {
        printf("\n--- MENU CADASTRO PRODUTO ---\n");
        printf("(1) Cadastrar\n(2) Alterar\n(3) Excluir\n(4) Ativar produto\n(0) Voltar\nEscolha: ");
        scanf("%d", &opcao);
        if(opcao==0){
            return;
        }
        if((opcao>0)&&(opcao<=4)){
            gerenciar[opcao-1]();
        }else{
            printf("Resposta inválida\n");
        }
    }while (opcao != 0);
}
