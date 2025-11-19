#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexico.h"

token token_atual;
FILE *arquivo = NULL;
FILE *arquivo_obj = NULL;
int caractere;

void enfileira(fila_tokens *fila, token t) {
    no *novo = malloc(sizeof(no));
    novo->t = t;
    novo->prox = NULL;

    if(fila->tras == NULL) {
        fila->frente = fila->tras = novo;
    } else {
        fila->tras->prox = novo;
        fila->tras = novo;
    }
}

void imprimir_lista_tokens(fila_tokens *fila) {
    no *atual = fila->frente;

    printf("\n Lista de Tokens\n");
    printf(" %-15s | %-15s\n", "Lexema", "Simbolo");
    printf("----------------------------------------\n");
    while(atual != NULL) {
        printf(" %-15s | %-15s\n", atual->t.lexema, atual->t.simbolo);
        atual = atual->prox;
    }
}

void trata_digito(fila_tokens *fila) {
    char num[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", caractere);
    strncat(num, temp, sizeof(num) - strlen(num) - 1);

    caractere = fgetc(arquivo);
    while(isdigit(caractere)) {
        snprintf(temp, sizeof(temp), "%c", caractere);
        strncat(num, temp, sizeof(num) - strlen(num) - 1);
        caractere = fgetc(arquivo);
    }

    strcpy(token_atual.lexema, num);
    strcpy(token_atual.simbolo, "snumero");
    enfileira(fila, token_atual);
}

void trata_identificador_palavra_reservada(fila_tokens *fila) {
    char id[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", caractere);
    caractere = fgetc(arquivo);

    while(isalpha(caractere) || isdigit(caractere) || caractere == '_') {
        strncat(id, temp, sizeof(id) - strlen(id) - 1);
        snprintf(temp, sizeof(temp), "%c", caractere);
        caractere = fgetc(arquivo);
    }

    strncat(id, temp, sizeof(id) - strlen(id) - 1);
    strcpy(token_atual.lexema, id);

    if (strcmp(id, "programa") == 0) strcpy(token_atual.simbolo, "sprograma");
    else if (strcmp(id, "se") == 0) strcpy(token_atual.simbolo, "sse");
    else if (strcmp(id, "entao") == 0) strcpy(token_atual.simbolo, "sentao");
    else if (strcmp(id, "senao") == 0) strcpy(token_atual.simbolo, "ssenao");
    else if (strcmp(id, "enquanto") == 0) strcpy(token_atual.simbolo, "senquanto");
    else if (strcmp(id, "faca") == 0) strcpy(token_atual.simbolo, "sfaca");
    else if (strcmp(id, "inicio") == 0) strcpy(token_atual.simbolo, "sinicio");
    else if (strcmp(id, "fim") == 0) strcpy(token_atual.simbolo, "sfim");
    else if (strcmp(id, "escreva") == 0) strcpy(token_atual.simbolo, "sescreva");
    else if (strcmp(id, "leia") == 0) strcpy(token_atual.simbolo, "sleia");
    else if (strcmp(id, "var") == 0) strcpy(token_atual.simbolo, "svar");
    else if (strcmp(id, "inteiro") == 0) strcpy(token_atual.simbolo, "sinteiro");
    else if (strcmp(id, "booleano") == 0) strcpy(token_atual.simbolo, "sbooleano");
    else if (strcmp(id, "verdadeiro") == 0) strcpy(token_atual.simbolo, "sverdadeiro");
    else if (strcmp(id, "falso") == 0) strcpy(token_atual.simbolo, "sfalso");
    else if (strcmp(id, "procedimento") == 0) strcpy(token_atual.simbolo, "sprocedimento");
    else if (strcmp(id, "funcao") == 0) strcpy(token_atual.simbolo, "sfuncao");
    else if (strcmp(id, "div") == 0) strcpy(token_atual.simbolo, "sdiv");
    else if (strcmp(id, "e") == 0) strcpy(token_atual.simbolo, "se");
    else if (strcmp(id, "ou") == 0) strcpy(token_atual.simbolo, "sou");
    else if (strcmp(id, "nao") == 0) strcpy(token_atual.simbolo, "snao");
    else strcpy(token_atual.simbolo, "sidentificador");

    enfileira(fila, token_atual);
}

void trata_atribuicao(fila_tokens *fila) {
    caractere = fgetc(arquivo);
    if(caractere == '=') {
        strcpy(token_atual.lexema, ":=");
        strcpy(token_atual.simbolo, "satribuicao");
        caractere = fgetc(arquivo);
    } else {
        strcpy(token_atual.lexema, ":");
        strcpy(token_atual.simbolo, "sdoispontos");
    }
    enfileira(fila, token_atual);
}

void trata_operador_aritmetico(fila_tokens *fila) {
    if(caractere == '+') strcpy(token_atual.simbolo, "smais");
    else if(caractere == '-') strcpy(token_atual.simbolo, "smenos");
    else if(caractere == '*') strcpy(token_atual.simbolo, "smult");

    snprintf(token_atual.lexema, sizeof(token_atual.lexema), "%c", caractere);
    caractere = fgetc(arquivo);
    enfileira(fila, token_atual);
}

void trata_operador_relacional(fila_tokens *fila) {
    if(caractere == '!') {
        caractere = fgetc(arquivo);
        if(caractere == '=') {
            caractere = fgetc(arquivo);
            strcpy(token_atual.lexema, "!=");
            strcpy(token_atual.simbolo, "sdif");
        } else printf("ERRO: esperado <!=>\n");
    }
    else if(caractere == '<') {
        caractere = fgetc(arquivo);
        if(caractere == '=') {
            caractere = fgetc(arquivo);
            strcpy(token_atual.lexema, "<=");
            strcpy(token_atual.simbolo, "smenorig");
        } else {
            strcpy(token_atual.lexema, "<");
            strcpy(token_atual.simbolo, "smenor");
        }
    }
    else if(caractere == '>') {
        caractere = fgetc(arquivo);
        if(caractere == '=') {
            caractere = fgetc(arquivo);
            strcpy(token_atual.lexema, ">=");
            strcpy(token_atual.simbolo, "smaiorig");
        } else {
            strcpy(token_atual.lexema, ">");
            strcpy(token_atual.simbolo, "smaior");
        }
    }
    else if(caractere == '=') {
        strcpy(token_atual.lexema, "=");
        strcpy(token_atual.simbolo, "sig");
        caractere = fgetc(arquivo);
    }

    enfileira(fila, token_atual);
}

void trata_pontuacao(fila_tokens *fila) {
    if(caractere == ';') strcpy(token_atual.simbolo, "sponto_virgula");
    else if(caractere == ',') strcpy(token_atual.simbolo, "svirgula");
    else if(caractere == '(') strcpy(token_atual.simbolo, "sabre_parenteses");
    else if(caractere == ')') strcpy(token_atual.simbolo, "sfecha_parenteses");
    else if(caractere == '.') strcpy(token_atual.simbolo, "sponto");

    snprintf(token_atual.lexema, sizeof(token_atual.lexema), "%c", caractere);
    caractere = fgetc(arquivo);
    enfileira(fila, token_atual);
}

void pega_token(fila_tokens *fila) {
    if (isdigit(caractere)) trata_digito(fila);
    else if (isalpha(caractere)) trata_identificador_palavra_reservada(fila);
    else if (caractere == ':') trata_atribuicao(fila);
    else if (caractere == '+' || caractere == '-' || caractere == '*') trata_operador_aritmetico(fila);
    else if (caractere == '!' || caractere == '<' || caractere == '>' || caractere == '=') trata_operador_relacional(fila);
    else if (caractere == ';' || caractere == ',' || caractere == '(' || caractere == ')' || caractere == '.') trata_pontuacao(fila);
    else {
        printf(" Erro: caractere inv�lido <%c>\n", caractere);
        caractere = fgetc(arquivo);
    }
}

int lexico(fila_tokens *fila) {
    while(caractere == ' ' || caractere == '\n' || caractere == '\t' || caractere == '{') {
        if(caractere == '{') {
            while(caractere != '}' && caractere != EOF) {
                caractere = fgetc(arquivo);
                if(caractere == EOF) {
                    printf(" Erro! Coment�rio n�o fechado\n");
                    return -1;
                }
            }
        }
        caractere = fgetc(arquivo);
    }

    if(caractere == EOF) return 2;

    pega_token(fila);
    return 1;
}
