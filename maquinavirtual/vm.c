/* 
 * Programa: vm.c
 * Descrição: Máquina virtual para executar arquivos.obj criados pelo compilador
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//Nó para pilha de valores/variáveis
struct no {
    int data;
    int endereco;
    struct no* prox;
};
typedef struct no No;

//Nó para lista de rotulo
struct noRotulo {
    char rotulo[8];
    int pos;
    struct noRotulo* prox;
};
typedef struct noRotulo NoRotulo;

//Nó para lista de instruções 
struct noInstrucao {
    char rotulo[8];
    char comando[9];
    char arg1[8];
    char arg2[8];
    struct noInstrucao* prox;
};
typedef struct noInstrucao NoInstrucao;


struct pilha {
    No* topo;
};
typedef struct pilha Pilha;

Pilha* mDados;
int programCounter = 0;
NoInstrucao* listaIntrucoes = NULL;
NoInstrucao* instrucaoAtual;
NoRotulo* listaRotulos = NULL;


/*
    Função buscaListIndex
    Descrição:
    Procura pela lista o elemento com base em sua posição
    Parâmetros:
    lista - lista para realizar a busca
    posicao - posicao do elemento que sera retornado

    Retorno:
    Retorna o nó se o elemento for encontrado, retorna NULL se não foi encontrado

*/
NoInstrucao* buscaListIndex(NoInstrucao* lista, int posicao){
    NoInstrucao *aux = lista;
    int contador = 0;
    //Procurando elemento pela lista
    while(aux != NULL){
        if(contador == posicao){
            return aux;
        }
        aux = aux->prox;
        contador++;
    }
    //Elemento não encontrado
    return NULL;
}


/*
    Função: listaAppend
    Descrição: 
    Adiciona um novo nó no final da lista de comandos
    Parâmetros:
    lista - lista onde novo elemento será adicionado
    rotulo - rótulo do comando
    comando - nome do comando
    arg1 - primeiro argumento do comando
    arg2 - segundo argumento do comando
    Retorno:
    lista com o elemento adicionado
    

*/
NoInstrucao* listaAppend(NoInstrucao* lista,
                         const char* rotulo,
                         const char* comando,
                         const char* arg1,
                         const char* arg2)
{

    //Alocando memória para nova instrução
    NoInstrucao *novo = (NoInstrucao*)malloc(sizeof(NoInstrucao));
    if(!novo){ perror("malloc"); exit(1); }

    //Copiando parâmetros para nova instrução
    memset(novo, 0, sizeof(NoInstrucao));
    strncpy(novo->rotulo,  rotulo ? rotulo : "",  sizeof(novo->rotulo)-1);
    strncpy(novo->comando, comando ? comando : "", sizeof(novo->comando)-1);
    strncpy(novo->arg1,    arg1    ? arg1    : "", sizeof(novo->arg1)-1);
    strncpy(novo->arg2,    arg2    ? arg2    : "", sizeof(novo->arg2)-1);

    novo->rotulo[7]  = '\0';
    novo->comando[8] = '\0';
    novo->arg1[7]    = '\0';
    novo->arg2[7]    = '\0';

    novo->prox = NULL;

    if (lista == NULL)
        return novo;

    //Encontrando último elemento
    NoInstrucao *aux = lista;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }
    //Colocando novo elemento
    aux->prox = novo;

    return lista;
}



/*
    Função: listaRotuloAppend
    Descrição: 
    Adiciona um novo nó no final da lista de rotulos
    Parâmetros:
    lista - lista onde novo elemento será adicionado
    rotulo - numero do rótulo em string
    pos - posição em que o rotulo está no arquivo (número da linha)
    Retorno:
    lista com o elemento adicionado

*/
NoRotulo* listaRotuloAppend(NoRotulo* lista,const char* rotulo,int pos)
{
    //Alocando espaço para novo elemento
    NoRotulo*novo = (NoRotulo*)malloc(sizeof(NoRotulo)); 
    if(!novo){ perror("malloc"); exit(1); }
    memset(novo, 0, sizeof(NoRotulo));
    
    //Copiando parâmetros
    strncpy(novo->rotulo,  rotulo ? rotulo : "",  sizeof(novo->rotulo)-1);
    novo->pos = pos;

    novo->rotulo[7]  = '\0';
    novo->prox = NULL;

    if (lista == NULL)
        return novo;

    //Buscando último elemento da lista
    NoRotulo *aux = lista;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }

    //Adicionando novo elemento
    aux->prox = novo;

    return lista;
}

/*
    Função buscaListRot
    Descrição:
    Procura pela lista o elemento com base em seu rotulo
    Parâmetros:
    lista - lista para realizar a busca
    rotulo - rotulo do elemento procurado
    Retorno:
    Retorna o nó se o elemento for encontrado, retorna NULL se não foi encontrado
*/

NoRotulo* buscaListRot(NoRotulo* lista, const char* rotulo){
    NoRotulo*aux = lista;
    //Percorrendo lista
    while(aux != NULL){
        if(strcmp(rotulo,aux->rotulo) == 0){
            //Rótulo encontrado
            return aux;
        }
        aux = aux->prox;
    }
    //Rótulo não encontrado
    return NULL;
}


/*
    Função criaPilha
    Descrição:
    Cria uma nova pilha de valores/variáveis
    Parâmetros:
    - Não tem parâmetros
    Retorno:
    Retorna a pilha criada

*/
Pilha* criaPilha()
{
    Pilha *p;
    p =(Pilha*)malloc(sizeof(Pilha));
    if(!p){ perror("malloc"); exit(1); }
    p->topo = NULL;
    return p;
}



/*
    Função ins_ini
    Descrição:
    Cria um novo elemento da pilha
    Parâmetros:
    t - topo da pilha
    a - dado do novo elemento da pilha
    end - endereço do novo elemento da pilha
    Retorno:
    Retorna o nó do novo elemento criado

*/

No *ins_ini(No*t,int a,int end){
    //Alocando espaço
    No *p = (No*)malloc(sizeof(No));
    if(!p){ perror("malloc"); exit(1); }
    //Colocando valores no novo elemento
    p->data = a;
    p->prox = t;
    p->endereco = end;
    return p;
}
/*
    Função push
    Descrição:
    Coloca um novo elemento no topo da pilha
    Parâmetros:
    p - Pilha
    v - dado do novo elemento da pilha
    end - endereço do novo elemento da pilha
    Retorno:
    não tem

*/
void push(Pilha *p, int v,int end)
{
    p->topo = ins_ini(p->topo,v,end);
}



/*

    Função ProcurarPilha
    Descrição:
    Procura um elemento da pilha com base no seu endereço
    Parâmetros:
    t - Pilha 
    end - Endereço do dado encontrado
    Retorno:
    Retorna o nó do elemento buscado, se não encontrado retorna NULL

*/
No* procurarPilha(Pilha *p,int end){
    No* aux = p->topo;
    //Procurando elemento
    while(aux != NULL){
        if(aux->endereco == end){
            //Elemento encontrado
            return aux;
        }
        aux = aux->prox;
    }
    //Elemento não encontrado
    return NULL;
}



/*
    Função pop
    Descrição:
    Tira um elemento do topo da pilha
    Parâmetros:
    p - Pilha
   
    Retorno:
    retorna o dado do elemento retirado

*/
int pop(Pilha *p){
    int v;
    if(p->topo == NULL){
        return 0;
    }
    v = p->topo->data;
    //Atualizando topo
    No*tmp = p->topo;
    p->topo = tmp->prox;
    free(tmp);
    //Retornando dado do elemento retirado
    return v;
}


/*
    Função executarLDC
    Descrição:
    Executa o comando LDC (coloca constante no topo da pilha)
    Parâmetros:
    arg - constante que será colocada
    Retorno:
    não tem
*/
 
void executarLDC(int arg){
    push(mDados,arg,-1);
}



/*
    Função executarALLOC
    Descrição:
    Executa o comando ALLOC (adiciona varíaveis para a pilha)
    Parâmetros:
    arg1 - endereço da primeira variável adicionada
    arg2 - número de variáveis adicionadas
    Retorno:
    não tem
*/
void executarALLOC(int arg1, int arg2){
    int k;
    No* aux;
    int conteudo = 0;

    //Adicionando arg2 variáveis
    for(k = 0; k < arg2; k++){
        aux = procurarPilha(mDados,arg1+k);
        //Se varíavel já existir, colocar conteudo da variável no topo (para função recursiva)
        if(aux != NULL){
            conteudo = aux->data;
            push(mDados,conteudo,-1);
        }else{
        //Se variável não existir adiciona nova variável na pilha
            push(mDados,conteudo,arg1+k);
        }
    }
}


/*
    Função executarDALLOC
    Descrição:
    Executa o comando DALLOC (retira variáveis do topo da pilha)
    
    Parâmetros:
    arg1 - endereço da primeira variável retirada
    arg2 - número de variáveis retiradas
    Retorno:
    não tem
*/
void executarDALLOC(int arg1,int arg2){
    int k;
    No* aux;
    int conteudo = 0;
    //Retirar arg2 variáveis
    for(k = arg2-1; k >= 0; k--){
        conteudo = mDados->topo->data;
        aux = procurarPilha(mDados,arg1+k);
        //Se variável ainda existir, colocar o conteúdo do topo na varíavel (Para função recursiva)
        if(aux != NULL){
            aux->data = conteudo;
        }
        //Retirando elemento do topo
        pop(mDados);
    }
}
/*
    Função executarADD
    Descrição:
    Executa o comando ADD, soma os dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarADD(){
    int conteudo = pop(mDados);
    if(mDados->topo) mDados->topo->data = mDados->topo->data + conteudo;
}


/*
    Função executarSUB
    Descrição:
    Executa o comando SUB, subtraí os dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarSUB(){
    int conteudo = pop(mDados);
    if(mDados->topo) mDados->topo->data = mDados->topo->data - conteudo;
}
/*
    Função executarMULT
    Descrição:
    Executa o comando MULT, multiplica os dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarMULT(){
    int conteudo = pop(mDados);
    if(mDados->topo) mDados->topo->data = mDados->topo->data * conteudo;
}
/*
    Função executarDIV
    Descrição:
    Executa o comando DIV, divide os dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarDIV(){
    int conteudo = pop(mDados);
    if(mDados->topo) mDados->topo->data = mDados->topo->data / conteudo;
}
/*
    Função executarINV
    Descrição:
    Executa o comando INV, inverte o elemento do topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarINV(){
    if(mDados->topo) mDados->topo->data = -mDados->topo->data;
}


/*
    Função executarAND
    Descrição:
    Executa o comando AND, realiza operação AND nos dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarAND(){
    int conteudo = pop(mDados);
    if(conteudo == 1 && mDados->topo->data == 1){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarOR
    Descrição:
    Executa o comando OR, realiza operação OR nos dois elementos mais altos da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarOR(){
    int conteudo = pop(mDados);
    if(conteudo == 1 || mDados->topo->data == 1){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}

/*
    Função executarNEG
    Descrição:
    Executa o comando NEG, nega o elemento do topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarNEG(){
    if(mDados->topo) mDados->topo->data = 1 - mDados->topo->data;
}


/*
    Função executarCME
    Descrição:
    Executa o comando CME, faz comparação ">" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCME(){
    int conteudo = pop(mDados);
    if(conteudo > mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarCMA
    Descrição:
    Executa o comando CMA, faz comparação "<" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCMA(){
    int conteudo = pop(mDados);
    if(conteudo < mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarCEQ
    Descrição:
    Executa o comando CEQ, faz comparação "==" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCEQ(){
    int conteudo = pop(mDados);
    if(conteudo == mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarCDIF
    Descrição:
    Executa o comando CDIF, faz comparação "!=" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCDIF(){
    int conteudo = pop(mDados);
    if(conteudo != mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarCDEQ
    Descrição:
    Executa o comando CMEQ, faz comparação ">=" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCMEQ(){
    int conteudo = pop(mDados);
    if(conteudo >= mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}
/*
    Função executarCMAQ
    Descrição:
    Executa o comando CMAQ, faz comparação "<=" com o elemento do topo e o próximo
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarCMAQ(){
    int conteudo = pop(mDados);
    if(conteudo <= mDados->topo->data ){
        mDados->topo->data = 1;
    }else{
         mDados->topo->data = 0;
    }
}


/*
    Função executarLDV
    Descrição:
    Executa o comando LDV, coloca o valor de uma variável no topo da pilha
    Parâmetros:
    arg1 - endereço da variável
    Retorno:
    não tem
*/
void executarLDV(int arg1){
    No* aux = procurarPilha(mDados,arg1);
    if(aux) push(mDados,aux->data,-1);
    else push(mDados,0,-1);
}
/*
    Função executarSTORE
    Descrição:
    Executa o comando STORE, coloca o valor do topo da pilha em uma variável
    Parâmetros:
    arg1 - endereço da variável
    Retorno:
    não tem
*/
void executarSTORE(int arg1){
    No* aux = procurarPilha(mDados,arg1);
    if(aux) aux->data = pop(mDados);
}
/*
    Função executarRD
    Descrição:
    Executa o comando RD, pede pro usuário escrever um valor, coloca o valor no topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarRD(){
    int conteudo;
    //Print para interface gráfica
    printf("[INPUT_REQUIRED]\n");
    fflush(stdout);
    //Lendo valor
    if(scanf("%d",&conteudo) == 1) {
        push(mDados,conteudo,-1);
    } else {
        push(mDados,0,-1);
    }
}
/*
    Função executarPRN
    Descrição:
    Executa o comando PRN, imprime o valor do topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarPRN(){
    int conteudo = pop(mDados);
    printf("\n%d\n",conteudo);
    fflush(stdout);
}
/*
    Função executarJMP
    Descrição:
    Executa o comando JMP, atribuí o valor do rotulo no programa counter
    Parâmetros:
    rotulo - rotulo para onde o pulo será realizado
    Retorno:
    não tem
*/
void executarJMP(char* rotulo){
    //Encontrando rótulo
    NoRotulo*rotuloEncontrado = buscaListRot(listaRotulos,rotulo);
    if(rotuloEncontrado){
        //Realizando pulo
        programCounter = rotuloEncontrado->pos;
        NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
        instrucaoAtual = instrucaoPulo;
    }
}
/*
    Função executarJMPF
    Descrição:
    Executa o comando JMPF, atribuí o valor do rotulo no programa counter se topo da pilha for 0
    Parâmetros:
    rotulo - rotulo para onde o pulo será realizado
    Retorno:
    não tem
*/
void executarJMPF(char* rotulo){
    int dado = pop(mDados);
    if(dado == 0){
        NoRotulo*rotuloEncontrado = buscaListRot(listaRotulos,rotulo);
        if(rotuloEncontrado){
            programCounter = rotuloEncontrado->pos;
            NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
            instrucaoAtual = instrucaoPulo;
        }
    }
}
/*
    Função executarCALL
    Descrição:
    Executa o comando CALL, realiza push do program counter na pilha e realiza pulo
    Parâmetros:
    rotulo - rotulo para onde o pulo será realizado
    Retorno:
    não tem
*/
void executarCALL(char* rotulo){
    push(mDados,programCounter,-1);
    executarJMP(rotulo);
}
/*
    Função executarRETURN
    Descrição:
    Executa o comando RETURN, pula para o valor que está no topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarRETURN(){
    int conteudo = pop(mDados);
    programCounter = conteudo;
    NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
    instrucaoAtual = instrucaoPulo;
}
/*
    Função executarRETURNF
    Descrição:
    Executa o comando RETURNF, pula para o valor que está no topo da pilha 
    e coloca valor do endereço 0 no topo da pilha
    Parâmetros:
    não tem
    Retorno:
    não tem
*/
void executarRETURNF(){
    executarRETURN();
    executarLDV(0);
}
/*
    Função executar
    Descrição:
    Seleciona qual comando será executado de arcordo com o nome do comando
    Parâmetros:
    inst - nome do comando
    rotulo - rotulo contido na linha do arquivo
    arg1 - argumento 1 do comando
    arg2 - argumento 2 do comando
    Retorno:
    não tem
*/
void executar(char *inst, char *rotulo, char* arg1, char* arg2) {
    if(strcmp(inst,"LDC") == 0){
        executarLDC(atoi(arg1));
    }else if(strcmp(inst,"ALLOC") == 0){
        executarALLOC(atoi(arg1),atoi(arg2));
    }else if(strcmp(inst,"LDV") == 0){
        executarLDV(atoi(arg1));
    }else if(strcmp(inst,"STR") == 0 || strcmp(inst,"STR ") == 0){
        executarSTORE(atoi(arg1));
    }else if(strcmp(inst,"DALLOC") == 0){
        executarDALLOC(atoi(arg1),atoi(arg2));
    }else if(strcmp(inst,"ADD") == 0){
        executarADD();
    }else if(strcmp(inst,"SUB") == 0){
        executarSUB();
    }else if(strcmp(inst,"MULT") == 0){
        executarMULT();
    }else if(strcmp(inst,"DIVI") == 0){
        executarDIV();
    }else if(strcmp(inst,"INV") == 0){
        executarINV();
    }else if(strcmp(inst,"AND") == 0){
        executarAND();
    }else if(strcmp(inst,"OR") == 0){
        executarOR();
    }else if(strcmp(inst,"NEG") == 0){
        executarNEG();
    }else if(strcmp(inst,"CME") == 0){
        executarCME();
    }else if(strcmp(inst,"CMA") == 0){
        executarCMA();
    }else if(strcmp(inst,"CEQ") == 0){
        executarCEQ();
    }else if(strcmp(inst,"CDIF") == 0){
        executarCDIF();
    }else if(strcmp(inst,"CMEQ") == 0){
        executarCMEQ();
    }else if(strcmp(inst,"CMAQ") == 0){
        executarCMAQ();
    }else if(strcmp(inst,"RD") == 0){
        executarRD();
    }else if(strcmp(inst,"PRN") == 0){
        executarPRN();
    }else if(strcmp(inst,"JMP") == 0){
        executarJMP(arg1);
    }else if(strcmp(inst,"JMPF") == 0){
        executarJMPF(arg1);
    }else if(strcmp(inst,"CALL") == 0){
        executarCALL(arg1);
    }else if(strcmp(inst,"RETURN") == 0){
        executarRETURN();
    }else if(strcmp(inst,"RETURNF") == 0){
        executarRETURNF();
    }
}

/*
    Função trim
    Descrição:
    Remove espaços em branco de uma string
    Parâmetros:
    s- string que terá os espaços removidos

    Retorno:
    não tem
*/
void trim(char *s) {
    int i = 0, j = (int)strlen(s) - 1;
    //procura espaço
    while (isspace((unsigned char)s[i])) i++;
    while (j >= i && isspace((unsigned char)s[j])) j--;
    //Move o conteúdo (sem espaços) para o incio da string
    memmove(s, s + i, j - i + 1);
    s[j - i + 1] = '\0';
}


/*
    Função executarInstrucao
    Descrição:
    Função para enviar orientações para interface gráfica (via print) e chamar a execução de instruções
    Parâmetros:
    Retorno:
    retorna 1
*/
int executarInstrucao() {
    
    if (instrucaoAtual == NULL) {
        //fim do arquivo
        printf("[END]\n");
        fflush(stdout);
        return 0;
    }
    if(strcmp(instrucaoAtual->comando,"HLT") == 0){
        //fim do arquivo (com comando HLT)
        printf("[END]\n");
        fflush(stdout);
        return 0;
    }

    //Envia instrução para interface gráfica
    printf("[INSTRUCTION] %d %s %s %s %s\n",
           programCounter,
           instrucaoAtual->rotulo[0] ? instrucaoAtual->rotulo : "-",
           instrucaoAtual->comando,
           instrucaoAtual->arg1,
           instrucaoAtual->arg2);
    fflush(stdout);

    //Executa instrução
    executar(instrucaoAtual->comando,
             instrucaoAtual->rotulo,
             instrucaoAtual->arg1,
             instrucaoAtual->arg2);

    //Indo para próxima instrução
    instrucaoAtual = instrucaoAtual->prox;
    programCounter++;


    //Enviando pilha para interface gráfica
    printf("[STACK]");
    No *aux5 = mDados->topo;
    while(aux5 != NULL){
        printf(" %d", aux5->data);
        aux5 = aux5->prox;
    }
    printf("\n");
    fflush(stdout);

    return 1;
}


/*
    Função main
    Descrição:
    Função para enviar orientações para interface gráfica (via print)
    Parâmetros:
    argc - número de argumentos
    argv - argumentos do programa (esperado, nome do arquivo e modo de execução )

    Retorno:
    retorna 0

*/

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    mDados = criaPilha();



    if (argc < 2) {
        printf("Uso: %s caminho_do_arquivo.obj [modo]\n", argv[0]);
        printf("modo: 0 = normal, 1 = console step (enter para próximo), 2 = GUI step (aguarda newline do GUI)\n");
        return 1;
    }

    int modo = 0;
    if (argc >= 3) modo = atoi(argv[2]);

    //Habilitando leitura do arquivo
    char *caminho = argv[1];
    FILE *file = fopen(caminho, "r");
    if (!file) {
        perror("Erro ao abrir arquivo .obj");
        return 1;
    }

    char line[128];
    int linha = 0;
    //Construindo lista de comandos
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        char rotulo[8]  = "";
        char comando[9] = "";
        char arg1[8]    = "";
        char arg2[8]    = "";

        //Separando palavras da linha do arquivo
        /*
        Primeiros 4 caracteres - rotulo
        Próximos 8 - comando
        Próximos 4 - argumento 1
        Próximos 4 - argumento 2
        */
        if ((int)strlen(line) >= 4) strncpy(rotulo,  line + 0,  4);
        if ((int)strlen(line) >= 12) strncpy(comando, line + 4,  8);
        if ((int)strlen(line) >= 16) strncpy(arg1,    line + 12, 4);
        if ((int)strlen(line) >= 20) strncpy(arg2,    line + 16, 4);

        rotulo[4] = '\0'; comando[8] = '\0'; arg1[4] = '\0'; arg2[4] = '\0';

        trim(rotulo);
        trim(comando);
        trim(arg1);
        trim(arg2);

        //Colocando rotulo na lista
        if (strlen(rotulo) > 0) {
            listaRotulos = listaRotuloAppend(listaRotulos, rotulo, linha);
        }

        //Colocando instrução na lista
        listaIntrucoes = listaAppend(listaIntrucoes, rotulo, comando, arg1, arg2);
        linha++;
    }

    fclose(file);

    instrucaoAtual = listaIntrucoes;


    //Começo da execução das intruções
    if(instrucaoAtual && strcmp(instrucaoAtual->comando,"START") == 0){

        //Modo 0 = execução direta
        if (modo == 0) {
            while (executarInstrucao()) {
            }
        } else if (modo == 1) { //Execução step para console
            while (executarInstrucao()) {
                printf("[WAIT]\n");
                fflush(stdout);
                getchar();
            }
        } else if (modo == 2) { //Execução step para interface gráfica
            while (executarInstrucao()) {
                printf("[STEP]\n");
                fflush(stdout);
                char buf[256];
                if (!fgets(buf, sizeof(buf), stdin)) {
                    break;
                }
            }
        } else { //Modo 0 como default
            while (executarInstrucao()) {}
        }
    } else {
        fprintf(stderr, "Arquivo .obj inválido: instrução START não encontrada.\n");
        return 1;
    }

    // finalizar
    printf("[END]\n");
    fflush(stdout);
    return 0;
}
