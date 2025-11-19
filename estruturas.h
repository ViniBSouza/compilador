#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>

typedef struct noOperador {
    char operador[10];
    struct noOperador* prox;
} NoOperador;

typedef struct pilha {
    NoOperador* topo;
} Pilha;

Pilha* criaPilha();
void push(Pilha* p, const char* op);
char* pop(Pilha* p);

#endif
