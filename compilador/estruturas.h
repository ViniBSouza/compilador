#ifndef ESTRUTURAS_H
#define ESTRUTURAS_H

#include <stdio.h>
#include "lexico.h"

/* Estrutura de um nó de operadores */
typedef struct noOperador {
    token t;
    struct noOperador* prox;
} NoOperador;

/* Estrutura de pilha auxiliar */
typedef struct pilha {
    NoOperador* topo;
} Pilha;

/* Estrutura de nó para tipo de expressão pós-fixa*/
typedef struct noTipo {
   char tipo[50];
   struct noTipo* prox;
} NoTipo;

/* Estrutura de pilha de tipo para expressão pós-fixa*/
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
