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

