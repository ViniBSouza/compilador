#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

typedef struct {
    char lexema[50];
    char simbolo[50];
} token;

typedef struct no {
    token t;
    struct no *prox;
} no;

typedef struct {
    no *frente;
    no *tras;
} fila_tokens;


extern token token_atual;
extern FILE *arquivo;
extern int caractere;


void enfileira(fila_tokens *fila, token t);
void imprimir_lista_tokens(fila_tokens *fila);
void trata_digito(fila_tokens *fila);
void trata_identificador_palavra_reservada(fila_tokens *fila);
void trata_atribuicao(fila_tokens *fila);
void trata_operador_aritmetico(fila_tokens *fila);
void trata_operador_relacional(fila_tokens *fila);
void trata_pontuacao(fila_tokens *fila);
void pega_token(fila_tokens *fila);
int lexico(fila_tokens *fila);

#endif
