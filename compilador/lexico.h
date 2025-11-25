#ifndef LEXICO_H
#define LEXICO_H

#include <stdio.h>

/* Estrutura de um token */
typedef struct {
    char lexema[50];
    char simbolo[50];
} token;

/* Estrutura de um nó de token */
typedef struct no {
    token t;
    struct no *prox;
} no;

/* Estrutura da fila de tokens */
typedef struct {
    no *frente;
    no *tras;
} fila_tokens;

/* Estrutura da lista de operadores para gerar expressão pós-fixa*/
typedef struct {
    token* operadores;
    int tamanho;
} ListaOperadores;

extern token token_atual;
extern FILE *arquivo;
extern FILE *arquivo_obj;
extern int caractere;
extern int linha;


void enfileira(token t);
void imprimir_lista_tokens();
void trata_digito();
void trata_identificador_palavra_reservada();
void trata_atribuicao();
void trata_operador_aritmetico();
void trata_operador_relacional();
void trata_pontuacao();
void pega_token();
int lexico();
void insere_lista(token novoToken, ListaOperadores* lista);

#endif
