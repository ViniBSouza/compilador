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


typedef struct noTipo {
   char tipo[50];
   struct noTipo* prox;
} NoTipo;


typedef struct {
    NoTipo* topo;
} PilhaTipo;


Pilha* criaPilha();
void push(Pilha* p, token t);
token pop(Pilha* p);
PilhaTipo* criaPilhaTipo();
void pushTipo(PilhaTipo* p, const char* tipo);
char* popTipo(PilhaTipo* p);


#endif
