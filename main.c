#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

void enfileira(fila_tokens *fila, token t) {
    no *novo = malloc(sizeof(no));
    novo-> t = t;
    novo-> prox = NULL;

    if(fila->tras == NULL) {
        fila -> frente = fila-> tras = novo;
    }
    else {
        fila-> tras-> prox = novo;
        fila->tras = novo;
    }
}

void imprimir_lista_tokens(fila_tokens *fila) {
    FILE *saida = fopen("lista_tokens.txt", "w");
    if(!saida) {
        printf("erro ao abrir arquivo");
        return;
    }

    no *atual = fila->frente;
    while(atual != NULL) {
        printf("%s - %s\n", atual->t.lexema, atual->t.simbolo);
        atual = atual->prox;
    }
    fclose(saida);
}

void trata_digito(FILE *arquivo, fila_tokens *fila, int *caractere) {
    char num[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", *caractere); // num <- caractere
    strncat(num, temp, sizeof(num) - strlen(num) - 1);

    *caractere = fgetc(arquivo);
    while(isdigit(*caractere)) {
        snprintf(temp, sizeof(temp), "%c", *caractere);
        strncat(num, temp, sizeof(num) - strlen(num) - 1); //num <- num + caractere
        *caractere = fgetc(arquivo);
    }

    token t;
    strcpy(t.lexema, num);
    strcpy(t.simbolo, "snumero");

    enfileira(fila, t);
}

void trata_identificador_palavra_reservada(FILE *arquivo, fila_tokens *fila, int *caractere) {
    char id[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", *caractere); // id <- caractere

    *caractere = fgetc(arquivo);
    while(isalpha(*caractere)||isdigit(*caractere)||*caractere == '_' ) {
        strncat(id, temp, sizeof(id) - strlen(id) - 1); // id <- id + caractere
        snprintf(temp, sizeof(temp), "%c", *caractere);
        *caractere = fgetc(arquivo);
    }

    strncat(id, temp, sizeof(id) - strlen(id) - 1); // id <- id + caractere
    token t;

    strcpy(t.lexema, id);

    if (strcmp(id, "programa") == 0) {
        strcpy(t.simbolo, "sprograma");
    }
    else if (strcmp(id, "se") == 0) {
        strcpy(t.simbolo, "sse");
    }
    else if (strcmp(id, "entao") == 0) {
        strcpy(t.simbolo, "sentao");
    }
    else if (strcmp(id, "senao") == 0) {
        strcpy(t.simbolo, "ssenao");
    }
    else if (strcmp(id, "enquanto") == 0) {
        strcpy(t.simbolo, "senquanto");
    }
    else if (strcmp(id, "faca") == 0) {
        strcpy(t.simbolo, "sfaca");
    }
    else if (strcmp(id, "inicio") == 0) {
        strcpy(t.simbolo, "sinicio");
    }
    else if (strcmp(id, "fim") == 0) {
        strcpy(t.simbolo, "sfim");
    }
    else if (strcmp(id, "escreva") == 0) {
        strcpy(t.simbolo, "sescreva");
    }
    else if (strcmp(id, "leia") == 0) {
        strcpy(t.simbolo, "sleia");
    }
    else if (strcmp(id, "var") == 0) {
        strcpy(t.simbolo, "svar");
    }
    else if (strcmp(id, "inteiro") == 0) {
        strcpy(t.simbolo, "sinteiro");
    }
    else if (strcmp(id, "booleano") == 0) {
        strcpy(t.simbolo, "sbooleano");
    }
    else if (strcmp(id, "verdadeiro") == 0) {
        strcpy(t.simbolo, "sverdadeiro");
    }
    else if (strcmp(id, "falso") == 0) {
        strcpy(t.simbolo, "sfalso");
    }
    else if (strcmp(id, "procedimento") == 0) {
        strcpy(t.simbolo, "sprocedimento");
    }
    else if (strcmp(id, "funcao") == 0) {
        strcpy(t.simbolo, "sfuncao");
    }
    else if (strcmp(id, "div") == 0) {
        strcpy(t.simbolo, "sdiv");
    }
    else if (strcmp(id, "ou") == 0) {
        strcpy(t.simbolo, "sou");
    }
    else if (strcmp(id, "nao") == 0) {
        strcpy(t.simbolo, "snao");
    }
    else {
        strcpy(t.simbolo, "sidentificador");
    }

    enfileira(fila, t);
}

void trata_atribuicao(FILE *arquivo, fila_tokens *fila, int *caractere) {

    token t;

    *caractere = fgetc(arquivo);

    if(*caractere == '=') {
        strcpy(t.lexema, ":=");
        strcpy(t.simbolo, "satribuicao");
        *caractere = fgetc(arquivo);
    }
    else {
        strcpy(t.lexema, ":");
        strcpy(t.simbolo, "sdoispontos");
    }

    enfileira(fila, t);
}

void trata_operador_aritmetico(FILE *arquivo, fila_tokens *fila, int *caractere) {

    token t;

    if(*caractere == '+') {
        strcpy(t.lexema, "+");
        strcpy(t.simbolo, "smais");
    }
    else if(*caractere == '-') {
        strcpy(t.lexema, "-");
        strcpy(t.simbolo, "smenos");
    }
    else if(*caractere == '*') {
        strcpy(t.lexema, "*");
        strcpy(t.simbolo, "smult");
    }
    *caractere = fgetc(arquivo);

    enfileira(fila, t);
}

void trata_operador_relacional(FILE *arquivo, fila_tokens *fila, int *caractere) {

    token t;

    if(*caractere == '!') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            strcpy(t.lexema, "!=");
            strcpy(t.simbolo, "sdif");
        }
        else
            printf("erro");
    }
    else if(*caractere == '<') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            strcpy(t.lexema, "<=");
            strcpy(t.simbolo, "smenorig");
        }
        else{
            strcpy(t.lexema, "<");
            strcpy(t.simbolo, "smenor");
        }
    }
    else if(*caractere == '>') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            strcpy(t.lexema, ">=");
            strcpy(t.simbolo, "smaiorig");
        }
        else{
            strcpy(t.lexema, ">");
            strcpy(t.simbolo, "smaior");
        }
    }
    else if(*caractere == '=') {
            strcpy(t.lexema, "=");
            strcpy(t.simbolo, "sig");
    }
    *caractere = fgetc(arquivo);
    enfileira(fila, t);
}

void trata_pontuacao(FILE *arquivo, fila_tokens *fila, int *caractere) {

    token t;

    if(*caractere == ';') {
        strcpy(t.lexema, ";");
        strcpy(t.simbolo, "sponto_virgula");
    }

	else if(*caractere == ',') {
        strcpy(t.lexema, ",");
        strcpy(t.simbolo, "svirgula");
	}

	else if (*caractere == '(') {
	    strcpy(t.lexema, "(");
        strcpy(t.simbolo, "sabre_parenteses");
	}

	else if(*caractere == ')') {
        strcpy(t.lexema, ")");
        strcpy(t.simbolo, "sfecha_parenteses");
	}
    else if(*caractere == '.') {
        strcpy(t.lexema, ".");
        strcpy(t.simbolo, "sponto");
    }

    *caractere = fgetc(arquivo);
    enfileira(fila, t);
}

void pega_token(FILE *arquivo, fila_tokens *fila, int *caractere) {
    if (isdigit(*caractere)) {
        trata_digito(arquivo, fila, caractere);
    }
    else if (isalpha(*caractere)) {
        trata_identificador_palavra_reservada(arquivo, fila, caractere);
    }
    else if (*caractere == ':') {
        trata_atribuicao(arquivo, fila, caractere);
    }
    else if (*caractere == '+' || *caractere == '-' || *caractere == '*') {
        trata_operador_aritmetico(arquivo, fila, caractere);
    }
    else if (*caractere == '!' || *caractere == '<' || *caractere == '>' || *caractere == '=') {
        trata_operador_relacional(arquivo, fila, caractere);
    }
    else if (*caractere == ';' || *caractere == ',' || *caractere == '(' || *caractere == ')' || *caractere == '.') {
        trata_pontuacao(arquivo, fila, caractere);
    }
    else {
        printf("erro\n");
    }
}

void analisador_lexical(FILE *arquivo, FILE *saida, fila_tokens *fila) {

    int c;  // precisa ser int, não char, para capturar fim de arquivo (EOF)

    // Abre o arquivo fonte
    arquivo = fopen("programa.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return; //return 1
    }

    saida = fopen("lista_tokens.txt", "w");
    if (saida == NULL) {
        printf("Erro ao abrir o arquivo de saída!\n");
        fclose(arquivo);
        return; //return 1
    }

    //Ler(caractere)
    c = fgetc(arquivo);

    // Lê caractere por caractere até o fim
    while (c != EOF) { //Enquanto não acabou o arquivo fonte
        if(c == ' ' || c == '\n' || c == '\t') {
            c = fgetc(arquivo);
            continue;
        }

        if(c == '{') {
            while(c != '}' && c != EOF) {
                c = fgetc(arquivo);
            }
            c = fgetc(arquivo);
            continue;
        }
        pega_token(arquivo, fila, &c);

    }

    // Fecha o arquivo
    fclose(arquivo);
    fclose(saida);
}

int main() {
    FILE *entrada = NULL, *arquivo_tokens = NULL;
    fila_tokens fila = {NULL, NULL};

    analisador_lexical(entrada, arquivo_tokens, &fila);
    imprimir_lista_tokens(&fila);

    return 0;
}
