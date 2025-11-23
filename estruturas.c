#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"
#include "lexico.h"


Pilha* criaPilha() {
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

void push(Pilha* p, token t) {
    NoOperador* novo = (NoOperador*)malloc(sizeof(NoOperador));
    if (!novo) {
        printf("Erro de alocação!\n");
        exit(1);
    }

    // copia o token inteiro
    novo->t = t;

    novo->prox = p->topo;
    p->topo = novo;
}


token pop(Pilha* p) {
    token vazio = {"", ""};

    if (p->topo == NULL) {
        printf("Pilha vazia!\n");
        return vazio;
    }

    NoOperador* tmp = p->topo;
    token t = tmp->t;

    p->topo = tmp->prox;
    free(tmp);

    return t;
}



PilhaTipo* criaPilhaTipo() {
    PilhaTipo* p = (PilhaTipo*) malloc(sizeof(PilhaTipo));
    if (!p) {
        printf("Erro de alocação!\n");
        exit(1);
    }

    p->topo = NULL;
    return p;
}


void pushTipo(PilhaTipo* p, const char* tipo) {
    NoTipo* novo = (NoTipo*) malloc(sizeof(NoTipo));
    if (!novo) {
        printf("Erro de alocação!\n");
        exit(1);
    }

    strcpy(novo->tipo, tipo);
    novo->prox = p->topo;
    p->topo = novo;
}


char* popTipo(PilhaTipo* p) {
    if (!p->topo) return NULL;

    NoTipo* tmp = p->topo;
    p->topo = tmp->prox;

    char* retorno = malloc(50);
    strcpy(retorno, tmp->tipo);

    free(tmp);
    return retorno;
}


