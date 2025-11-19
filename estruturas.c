#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estruturas.h"

Pilha* criaPilha() {
    Pilha* p = (Pilha*)malloc(sizeof(Pilha));
    p->topo = NULL;
    return p;
}

void push(Pilha* p, const char* op) {
    NoOperador* novo = (NoOperador*)malloc(sizeof(NoOperador));
    strcpy(novo->operador, op);
    novo->prox = p->topo;
    p->topo = novo;
}

char* pop(Pilha* p) {
    if (p->topo == NULL) {
        printf("pilha vazia\n");
        return NULL;
    }

    NoOperador* tmp = p->topo;
    char* op = strdup(tmp->operador);
    p->topo = tmp->prox;
    free(tmp);

    return op;
}
