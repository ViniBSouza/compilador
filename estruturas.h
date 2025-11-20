#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include "lexico.h"




typedef struct noOperador {
    token t;
    struct noOperador* prox;
} NoOperador;

typedef struct pilha {
    NoOperador* topo;
} Pilha;



Pilha* criaPilha();
void push(Pilha* p, token t);
token pop(Pilha* p);


#endif
