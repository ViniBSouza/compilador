#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct no
{
    int data;
    int endereco;
    struct no* prox;
};
typedef struct no No;


struct noRotulo
{
    char rotulo[8];
    int pos;
    struct noRotulo* prox;

};
typedef struct noRotulo NoRotulo;

struct noInstrucao
{
    char rotulo[8];
    char comando[9];
    char arg1[8];
    char arg2[8];
    struct noInstrucao* prox;
};
typedef struct noInstrucao NoInstrucao;

struct pilha
{
    No* topo;
};
typedef struct pilha Pilha;



Pilha* mDados;
int programCounter = 0;
NoInstrucao* listaIntrucoes = NULL;
NoInstrucao* instrucaoAtual;
NoRotulo* listaRotulos = NULL;


NoInstrucao* buscaListIndex(NoInstrucao* lista, int posicao){
    NoInstrucao *aux = lista;
    int contador = 0;
    while(aux != NULL){
        if(contador == posicao){
            return aux;
        }
        aux = aux->prox;
        contador++;
    }
    return NULL;
}

NoInstrucao* listaAppend(NoInstrucao* lista,
                         const char* rotulo,
                         const char* comando,
                         const char* arg1,
                         const char* arg2)
{
    NoInstrucao *novo = malloc(sizeof(NoInstrucao));

    strncpy(novo->rotulo,  rotulo,  sizeof(novo->rotulo)-1);
    strncpy(novo->comando, comando, sizeof(novo->comando)-1);
    strncpy(novo->arg1,    arg1,    sizeof(novo->arg1)-1);
    strncpy(novo->arg2,    arg2,    sizeof(novo->arg2)-1);

    novo->rotulo[7]  = '\0';
    novo->comando[8] = '\0';
    novo->arg1[7]    = '\0';
    novo->arg2[7]    = '\0';

    novo->prox = NULL;

    if (lista == NULL)
        return novo;

    NoInstrucao *aux = lista;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }

    aux->prox = novo;

    return lista;
}


NoRotulo* listaRotuloAppend(NoRotulo* lista,char* rotulo,int pos)
{
    NoRotulo*novo = malloc(sizeof(NoInstrucao));

    strncpy(novo->rotulo,  rotulo,  sizeof(novo->rotulo)-1);
    novo->pos = pos;


    novo->rotulo[7]  = '\0';
    novo->prox = NULL;

    if (lista == NULL)
        return novo;

    NoRotulo *aux = lista;
    while (aux->prox != NULL) {
        aux = aux->prox;
    }

    aux->prox = novo;

    return lista;
}



NoRotulo* buscaListRot(NoRotulo* lista, char* rotulo){
    NoRotulo*aux = lista;
    while(aux != NULL){
        if(strcmp(rotulo,aux->rotulo) == 0){
            return aux;
        }
        aux = aux->prox;
    }
    return NULL;
}

Pilha* criaPilha()
{
    Pilha *p;
    p =(Pilha*)malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

No *ins_ini(No*t,int a,int end){
    No *p = (No*)malloc(sizeof(No));
    p->data = a;
    p->prox = t;
    p->endereco = end;
    return p;
}


void push(Pilha *p, int v,int end)
{
    p->topo = ins_ini(p->topo,v,end);

}



No* procurarPilha(Pilha *p,int end){

    No* aux = p->topo;
    while(aux != NULL){
        if(aux->endereco == end){
            return aux;
        }
        aux = aux->prox;
    }
    return NULL;
}


int pop(Pilha *p){

        int v;
        if(p->topo == NULL){
            printf("pilha vazia");
            return 0;
        }
        v = p->topo->data;
        No*tmp = p->topo;

        p-> topo = tmp->prox;
        free(tmp);
        return v;
}


void executarLDC(int arg){
    push(mDados,arg,-1);

}

void executarALLOC(int arg1, int arg2){
    int k;

    No* aux;
    int conteudo = 0;

    for(k = 0; k < arg2; k++){

        aux = procurarPilha(mDados,arg1+k);
        if(aux != NULL){
            conteudo = aux->data;
        }
        push(mDados,conteudo,arg1+k);
    }

}


void executarDALLOC(int arg1,int arg2){
    int k;
    No* aux;
    int conteudo = 0;

    for(k = arg2-1; k >= 0; k--){
        conteudo = pop(mDados);
        aux = procurarPilha(mDados,arg1+k);
        if(aux != NULL){
            aux->data = conteudo;
        }

    }

}

void executarADD(){
    int conteudo = pop(mDados);
    mDados->topo->data = mDados->topo->data + conteudo;

}

void executarSUB(){
    int conteudo = pop(mDados);
    mDados->topo->data = mDados->topo->data - conteudo;

}

void executarMULT(){
    int conteudo = pop(mDados);
    mDados->topo->data = mDados->topo->data * conteudo;

}

void executarDIV(){
    int conteudo = pop(mDados);
    mDados->topo->data = mDados->topo->data / conteudo;

}

void executarINV(){
    mDados->topo->data = -mDados->topo->data;

}


void executarAND(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo == 1 && mDados->topo->data == 1){
        mDados->topo->data = 1;
    }
}

void executarOR(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo == 1 || mDados->topo->data == 1){
        mDados->topo->data = 1;
    }
}

void executarNEG(){
    mDados->topo->data = 1 - mDados->topo->data;
}


void executarCME(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo > mDados->topo->data ){
        mDados->topo->data = 1;
    }
}

void executarCMA(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo < mDados->topo->data ){
        mDados->topo->data = 1;
    }
}

void executarCEQ(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo == mDados->topo->data ){
        mDados->topo->data = 1;
    }
}

void executarCDIF(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo != mDados->topo->data ){
        mDados->topo->data = 1;
    }
}


void executarCMEQ(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo >= mDados->topo->data ){
        mDados->topo->data = 1;
    }
}

void executarCMAQ(){
    int conteudo = pop(mDados);
    mDados->topo->data = 0;
    if(conteudo <= mDados->topo->data ){
        mDados->topo->data = 1;
    }
}



void executarLDV(int arg1){
    No* aux = procurarPilha(mDados,arg1);
    push(mDados,aux->data,-1);
}

void executarSTORE(int arg1){
    No* aux = procurarPilha(mDados,arg1);
    aux->data = pop(mDados);
}

void executarRD(){
    int conteudo;
    scanf("%d",&conteudo);
    push(mDados,conteudo,-1);
}

void executarPRN(){
    int conteudo = pop(mDados);
    printf("\n%d\n",conteudo);

}

void executarJMP(char* rotulo){
    NoRotulo*rotuloEncontrado = buscaListRot(listaRotulos,rotulo);
    programCounter = rotuloEncontrado->pos;
    NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
    instrucaoAtual = instrucaoPulo;

}

void executarJMPF(char* rotulo){
    int dado = pop(mDados);
    if(dado == 0){
        NoRotulo*rotuloEncontrado = buscaListRot(listaRotulos,rotulo);
        programCounter = rotuloEncontrado->pos;
        NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
        instrucaoAtual = instrucaoPulo;

    }

}

void executarCALL(char* rotulo){
    push(mDados,programCounter,-1);
    executarJMP(rotulo);
}

void executarRETURN(){
     int conteudo = pop(mDados);
     printf("\n Vou pular para %d\n", conteudo);

     programCounter = conteudo;
     NoInstrucao*instrucaoPulo = buscaListIndex(listaIntrucoes,programCounter);
     instrucaoAtual = instrucaoPulo;

}


void executarRETURNF(){
    executarRETURN();
    executarLDV(0);
}

void executar(char *inst, char *rotulo, char* arg1, char* arg2) {

    printf("\n Executando %s", inst);

    if(strcmp(inst,"LDC") == 0){
        executarLDC(atoi(arg1));
    }else if(strcmp(inst,"ALLOC") == 0){
        executarALLOC(atoi(arg1),atoi(arg2));
    }else if(strcmp(inst,"LDV") == 0){
        executarLDV(atoi(arg1));
    }else if(strcmp(inst,"STR") == 0){
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

//
}
void trim(char *s) {
    // remove spaces leading and trailing
    int i = 0, j = strlen(s) - 1;

    // remove espaços no início
    while (isspace((unsigned char)s[i])) i++;

    // remove espaços no fim
    while (j >= i && isspace((unsigned char)s[j])) j--;

    // desloca
    memmove(s, s + i, j - i + 1);
    s[j - i + 1] = '\0';
}

int main(int argc, char *argv[]) {

    mDados = criaPilha();
    /*
    if (argc < 2) {
        printf("Uso: %s caminho_do_arquivo.obj\n", argv[0]);
        return 1;
    }

    char *caminho = argv[1];
    */

    char *caminho = "arquivo.obj";
    FILE *file = fopen(caminho, "r");
    if (!file) {
        perror("Erro ao abrir arquivo .obj");
        return 1;
    }


    char line[34];
    int linha = 0;
    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\n")] = 0;  // remove \n

        char rotulo[8]  = "";
        char comando[9] = "";
        char arg1[8]    = "";
        char arg2[8]    = "";

        // --- COPIAR PELO TAMANHO FIXO DE CADA CAMPO ---
        strncpy(rotulo,  line + 0,  4); rotulo[4]  = '\0';
        strncpy(comando, line + 4,  8); comando[8] = '\0';
        strncpy(arg1,    line + 12, 4); arg1[4]    = '\0';
        strncpy(arg2,    line + 16, 4); arg2[4]    = '\0';

        // --- REMOVER ESPAÇOS EXTRAS ---
        trim(rotulo);
        trim(comando);
        trim(arg1);
        trim(arg2);

        // --- SE ROTULO NÃO ESTIVER VAZIO, É RÓTULO ---
        if (strlen(rotulo) > 0) {
            listaRotulos = listaRotuloAppend(listaRotulos, rotulo, linha);
        }

        printf("ROTULO='%s' CMD='%s' ARG1='%s' ARG2='%s'\n",
               rotulo, comando, arg1, arg2);

        listaIntrucoes = listaAppend(listaIntrucoes, rotulo, comando, arg1, arg2);




        linha++;
    }

    fclose(file);

    instrucaoAtual = listaIntrucoes;
    if(strcmp(instrucaoAtual->comando,"START") == 0){
        while(strcmp(instrucaoAtual->comando,"HLT") != 0){
            executar(instrucaoAtual->comando,instrucaoAtual->rotulo,
                     instrucaoAtual->arg1,instrucaoAtual->arg2);
            instrucaoAtual = instrucaoAtual->prox;
            programCounter++;

                    No *aux5;
        aux5 = mDados->topo;
        printf("Pilha: \n");
        while(aux5 != NULL){
            printf("\n %d", aux5->data);
            aux5 = aux5->prox;
        }

        }
    }




    printf("\nExecucao finalizada.\n");
    No *aux;
    aux = mDados->topo;
    printf("Pilha: \n");
    while(aux != NULL){
        printf("\n %d", aux->data);
        aux = aux->prox;
    }


    printf("\nIntrucoes: \n");
    NoInstrucao *aux1 = listaIntrucoes;
    while(aux1 != NULL){
        printf("ROTULO: '%s', CMD: '%s', ARG1: '%s', ARG2: '%s'\n",
           aux1->rotulo, aux1->comando, aux1->arg1, aux1->arg2);
        aux1 = aux1->prox;
    }

    printf("Lista de rotulos:\n");
    NoRotulo *aux2 = listaRotulos;
    while(aux2 != NULL){
        printf("\n %s", aux2->rotulo);
        aux2 = aux2->prox;
    }
    return 0;

}
