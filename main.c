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

// ====== Prototipos Analisador sintatico ======
void analisa_comandos(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_comando_simples(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_expressao_simples(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_termo(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_fator(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_declaracao_procedimento(token *t, FILE *arquivo, fila_tokens *fila, int *caractere);
void analisa_subrotinas(token *t, FILE *arquivo, fila_tokens *fila, int* caractere);
void analisa_bloco(token *t, FILE *arquivo, fila_tokens *fila, int* caractere);

void enfileira(fila_tokens *fila, token t) {
    no *novo = malloc(sizeof(no));
    novo->t = t;
    novo->prox = NULL;

    if(fila->tras == NULL) {
        fila->frente = fila->tras = novo;
    }
    else {
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

void trata_digito(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    char num[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", *caractere);
    strncat(num, temp, sizeof(num) - strlen(num) - 1);

    *caractere = fgetc(arquivo);
    while(isdigit(*caractere)) {
        snprintf(temp, sizeof(temp), "%c", *caractere);
        strncat(num, temp, sizeof(num) - strlen(num) - 1);
        *caractere = fgetc(arquivo);
    }

    strcpy(t->lexema, num);
    strcpy(t->simbolo, "snumero");

    enfileira(fila, *t);
}

void trata_identificador_palavra_reservada(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    char id[20] = "";
    char temp[2];

    snprintf(temp, sizeof(temp), "%c", *caractere);

    *caractere = fgetc(arquivo);
    while(isalpha(*caractere) || isdigit(*caractere) || *caractere == '_' ) {
        strncat(id, temp, sizeof(id) - strlen(id) - 1);
        snprintf(temp, sizeof(temp), "%c", *caractere);
        *caractere = fgetc(arquivo);
    }

    strncat(id, temp, sizeof(id) - strlen(id) - 1);

    strcpy(t->lexema, id);

    if (strcmp(id, "programa") == 0) {
        strcpy(t->simbolo, "sprograma");
    }
    else if (strcmp(id, "se") == 0) {
        strcpy(t->simbolo, "sse");
    }
    else if (strcmp(id, "entao") == 0) {
        strcpy(t->simbolo, "sentao");
    }
    else if (strcmp(id, "senao") == 0) {
        strcpy(t->simbolo, "ssenao");
    }
    else if (strcmp(id, "enquanto") == 0) {
        strcpy(t->simbolo, "senquanto");
    }
    else if (strcmp(id, "faca") == 0) {
        strcpy(t->simbolo, "sfaca");
    }
    else if (strcmp(id, "inicio") == 0) {
        strcpy(t->simbolo, "sinicio");
    }
    else if (strcmp(id, "fim") == 0) {
        strcpy(t->simbolo, "sfim");
    }
    else if (strcmp(id, "escreva") == 0) {
        strcpy(t->simbolo, "sescreva");
    }
    else if (strcmp(id, "leia") == 0) {
        strcpy(t->simbolo, "sleia");
    }
    else if (strcmp(id, "var") == 0) {
        strcpy(t->simbolo, "svar");
    }
    else if (strcmp(id, "inteiro") == 0) {
        strcpy(t->simbolo, "sinteiro");
    }
    else if (strcmp(id, "booleano") == 0) {
        strcpy(t->simbolo, "sbooleano");
    }
    else if (strcmp(id, "verdadeiro") == 0) {
        strcpy(t->simbolo, "sverdadeiro");
    }
    else if (strcmp(id, "falso") == 0) {
        strcpy(t->simbolo, "sfalso");
    }
    else if (strcmp(id, "procedimento") == 0) {
        strcpy(t->simbolo, "sprocedimento");
    }
    else if (strcmp(id, "funcao") == 0) {
        strcpy(t->simbolo, "sfuncao");
    }
    else if (strcmp(id, "div") == 0) {
        strcpy(t->simbolo, "sdiv");
    }
    else if (strcmp(id, "e") == 0) {
        strcpy(t->simbolo, "se");
    }
    else if (strcmp(id, "ou") == 0) {
        strcpy(t->simbolo, "sou");
    }
    else if (strcmp(id, "nao") == 0) {
        strcpy(t->simbolo, "snao");
    }
    else {
        strcpy(t->simbolo, "sidentificador");
    }
    enfileira(fila, *t);
}

void trata_atribuicao(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    *caractere = fgetc(arquivo);

    if(*caractere == '=') {
        strcpy(t->lexema, ":=");
        strcpy(t->simbolo, "satribuicao");
        *caractere = fgetc(arquivo);
    }
    else {
        strcpy(t->lexema, ":");
        strcpy(t->simbolo, "sdoispontos");
    }

    enfileira(fila, *t);
}

void trata_operador_aritmetico(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    if(*caractere == '+') {
        strcpy(t->lexema, "+");
        strcpy(t->simbolo, "smais");
    }
    else if(*caractere == '-') {
        strcpy(t->lexema, "-");
        strcpy(t->simbolo, "smenos");
    }
    else if(*caractere == '*') {
        strcpy(t->lexema, "*");
        strcpy(t->simbolo, "smult");
    }
    *caractere = fgetc(arquivo);

    enfileira(fila, *t);
}

void trata_operador_relacional(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    if(*caractere == '!') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            *caractere = fgetc(arquivo);
            strcpy(t->lexema, "!=");
            strcpy(t->simbolo, "sdif");
        }
        else
            printf("ERRO: esperado <!=>\n");
    }
    else if(*caractere == '<') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            *caractere = fgetc(arquivo);
            strcpy(t->lexema, "<=");
            strcpy(t->simbolo, "smenorig");
        }
        else{
            strcpy(t->lexema, "<");
            strcpy(t->simbolo, "smenor");
        }
    }
    else if(*caractere == '>') {
        *caractere = fgetc(arquivo);
        if(*caractere == '=') {
            *caractere = fgetc(arquivo);
            strcpy(t->lexema, ">=");
            strcpy(t->simbolo, "smaiorig");
        }
        else{
            strcpy(t->lexema, ">");
            strcpy(t->simbolo, "smaior");
        }
    }
    else if(*caractere == '=') {
        strcpy(t->lexema, "=");
        strcpy(t->simbolo, "sig");
        *caractere = fgetc(arquivo);
    }

    enfileira(fila, *t);
}

void trata_pontuacao(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    if(*caractere == ';') {
        strcpy(t->lexema, ";");
        strcpy(t->simbolo, "sponto_virgula");
    }
    else if(*caractere == ',') {
        strcpy(t->lexema, ",");
        strcpy(t->simbolo, "svirgula");
    }
    else if (*caractere == '(') {
        strcpy(t->lexema, "(");
        strcpy(t->simbolo, "sabre_parenteses");
    }
    else if(*caractere == ')') {
        strcpy(t->lexema, ")");
        strcpy(t->simbolo, "sfecha_parenteses");
    }
    else if(*caractere == '.') {
        strcpy(t->lexema, ".");
        strcpy(t->simbolo, "sponto");
    }

    *caractere = fgetc(arquivo);
    enfileira(fila, *t);
}

void pega_token(FILE *arquivo, fila_tokens *fila, int *caractere, token *t) {
    if (isdigit(*caractere)) {
        trata_digito(arquivo, fila, caractere, t);
    }
    else if (isalpha(*caractere)) {
        trata_identificador_palavra_reservada(arquivo, fila, caractere, t);
    }
    else if (*caractere == ':') {
        trata_atribuicao(arquivo, fila, caractere, t);
    }
    else if (*caractere == '+' || *caractere == '-' || *caractere == '*') {
        trata_operador_aritmetico(arquivo, fila, caractere, t);
    }
    else if (*caractere == '!' || *caractere == '<' || *caractere == '>' || *caractere == '=') {
        trata_operador_relacional(arquivo, fila, caractere, t);
    }
    else if (*caractere == ';' || *caractere == ',' || *caractere == '(' || *caractere == ')' || *caractere == '.') {
        trata_pontuacao(arquivo, fila, caractere, t);
    }
    else {
        printf(" Erro ao pegar token, caractere <%c> invalido\n", *caractere);
        *caractere = fgetc(arquivo);
    }
}

int lexico(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    while(*caractere == ' ' || *caractere == '\n' || *caractere == '\t' || *caractere == '{') {
        if(*caractere == '{') {
            while(*caractere != '}' && *caractere != EOF) {
                *caractere = fgetc(arquivo);
                if(*caractere == EOF && *caractere != '}') {
                    printf(" Erro! Necessario fechar comentario com <}>");
                    return -1;
                }
            }
        }
        *caractere = fgetc(arquivo);
    }

    if(*caractere == EOF){
        return 2;
    }

    pega_token(arquivo, fila, caractere, t);
    return 1;
}

void analisa_chamada_funcao(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "sidentificador") != 0) {
        printf("ERRO analisa_chamada_funcao: esperado identificador\n");
    }
}

void analisa_chamada_procedimento(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    //if(strcmp(t->simbolo, "sidentificador") != 0) {
    //    printf("ERRO analisa_chamada_procedimento: esperado identificador\n");
    //}
}

void analisa_expressao(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    analisa_expressao_simples(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "smaior") == 0 || strcmp(t->simbolo, "smaiorig") == 0 ||
       strcmp(t->simbolo, "sig") == 0 || strcmp(t->simbolo, "smenor") == 0 ||
       strcmp(t->simbolo, "smenorig") == 0 || strcmp(t->simbolo, "sdif") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_expressao_simples(t, arquivo, fila, caractere);
       }
}

void analisa_atribuicao(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    analisa_expressao(t, arquivo, fila, caractere);
}

void analisa_tipo(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "sinteiro") != 0 && strcmp(t->simbolo, "sbooleano") != 0) {
        printf("ERRO analisa_tipo: tipo de variavel invalida\n");
    }
    lexico(t, arquivo, fila, caractere);
}

void analisa_leia(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sabre_parenteses") == 0) {
        lexico(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sidentificador") == 0) {
            lexico(t, arquivo, fila, caractere);
            if(strcmp(t->simbolo, "sfecha_parenteses") == 0) {
                lexico(t, arquivo, fila, caractere);
            }
            else {
                printf("ERRO analisa_leia: esperado <)>\n");
            }
        }
        else {
            printf("ERRO analisa_leia: esperado identificador\n");
        }
    }
    else {
        printf("ERRO analisa_leia: esperado <(>\n");
    }
}

void analisa_escreva(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sabre_parenteses") == 0) {
        lexico(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sidentificador") == 0) {
            lexico(t, arquivo, fila, caractere);
            if(strcmp(t->simbolo, "sfecha_parenteses") == 0) {
                lexico(t, arquivo, fila, caractere);
            }
            else {
                printf("ERRO analisa_escreva: esperado <)>\n");
            }
        }
        else {
            printf("ERRO analisa_escreva: esperado identificador\n");
        }
    }
    else {
        printf("ERRO analisa_escreva: esperado <(>\n");
    }
}

void analisa_variaveis(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    while(strcmp(t->simbolo, "sdoispontos") != 0) {
        if(strcmp(t->simbolo, "sidentificador") == 0) {
            lexico(t, arquivo, fila, caractere);
            if(strcmp(t->simbolo, "svirgula") == 0 || strcmp(t->simbolo, "sdoispontos") == 0) {
                if(strcmp(t->simbolo, "svirgula") == 0) {
                    lexico(t, arquivo, fila, caractere);
                    if(strcmp(t->simbolo, "sdoispontos") == 0) {
                        printf("ERRO analisa_variaveis\n");
                        exit(1);
                    }
                }
            }
            else {
                printf("ERRO analisa_variaveis: esperado <,> ou <:>\n");
            }
        }
        else {
            printf("ERRO analisa_variaveis: esperado identificador\n");
        }
    }
    lexico(t, arquivo, fila, caractere);
    analisa_tipo(t, arquivo, fila, caractere);
}

void analisa_et_variaveis(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "svar") == 0) {
        lexico(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sidentificador") == 0) {
            while(strcmp(t->simbolo, "sidentificador") == 0) {
                analisa_variaveis(t, arquivo, fila, caractere);
                if(strcmp(t->simbolo, "sponto_virgula") == 0) {
                    lexico(t, arquivo, fila, caractere);
                }
                else {
                    printf("ERRO analisa_et_variaveis: esperado <;>\n");
                }
            }
        }
        else {
            printf("ERRO analisa_et_variaveis: esperado identificador\n");
        }
    }
}

void analisa_atrib_chprocedimento(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "satribuicao") == 0) {
        analisa_atribuicao(t, arquivo, fila, caractere);
    }
    else {
        analisa_chamada_procedimento(t, arquivo, fila, caractere);
    }
}

void analisa_se(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    analisa_expressao(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sentao") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_comando_simples(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "ssenao") == 0) {
            lexico(t, arquivo, fila, caractere);
            analisa_comando_simples(t, arquivo, fila, caractere);
        }
    }
    else {
        printf("ERRO analisa_se: esperado <entao>\n");
    }
}

void analisa_enquanto(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    analisa_expressao(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sfaca") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_comando_simples(t, arquivo, fila, caractere);
    }
    else {
        printf("ERRO analisa_enquanto: esperado <faca>\n");
    }
}

void analisa_declaracao_procedimento(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sidentificador") == 0) {
        lexico(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sponto_virgula") == 0) {
            analisa_bloco(t, arquivo, fila, caractere);
        }
        else {
            printf("ERRO analisa_declaracao_procedimento: esperado <;>\n");
        }
    }
    else {
        printf("ERRO analisa_declaracao_procedimento: esperado identificador\n");
    }
}

void analisa_declaracao_funcao(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    if(strcmp(t->simbolo, "sidentificador") == 0) {
        lexico(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sdoispontos") == 0) {
            lexico(t, arquivo, fila, caractere);
            if(strcmp(t->simbolo, "sinteiro") == 0 || strcmp(t->simbolo, "sbooleano") == 0) {
                lexico(t, arquivo, fila, caractere);
                if(strcmp(t->simbolo, "sponto_virgula") == 0) {
                    analisa_bloco(t, arquivo, fila, caractere);
                }
            }
            else {
                printf("ERRO analisa_declaracao_funcao: tipo invalido\n");
            }
        }
        else {
            printf("ERRO analisa_declaracao_funcao: esperado <:>\n");
        }
    }
    else {
        printf("ERRO analisa_declaracao_funcao: esperado identificador\n");
    }
}

void analisa_subrotinas(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    while(strcmp(t->simbolo, "sprocedimento") == 0 || strcmp(t->simbolo, "sfuncao") == 0) {
        if(strcmp(t->simbolo, "sprocedimento") == 0) {
            analisa_declaracao_procedimento(t, arquivo, fila, caractere);
        }
        else {
            analisa_declaracao_funcao(t, arquivo, fila, caractere);
        }

        if(strcmp(t->simbolo, "sponto_virgula") == 0) {
            lexico(t, arquivo, fila, caractere);
        }
        else {
            printf("ERRO analisa_subrotinas: esperado <;>\n");
        }
    }
}

void analisa_bloco(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    lexico(t, arquivo, fila, caractere);
    analisa_et_variaveis(t, arquivo, fila, caractere);
    analisa_subrotinas(t, arquivo, fila, caractere);
    analisa_comandos(t, arquivo, fila, caractere);
}

void analisa_fator(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "sidentificador") == 0) {
        analisa_chamada_funcao(t, arquivo, fila, caractere);
        lexico(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "snumero") == 0) {
        lexico(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "snao") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_fator(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "sabre_parenteses") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_expressao(t, arquivo, fila, caractere);
        if(strcmp(t->simbolo, "sfecha_parenteses") == 0) {
            lexico(t, arquivo, fila, caractere);
        }
        else {
            printf("ERRO analisa_fator: esperado <)>\n");
        }
    }
    else if(strcmp(t->lexema, "verdadeiro") == 0 || strcmp(t->lexema, "falso") == 0) {
        lexico(t, arquivo, fila, caractere);
    }
    else {
        printf("%s", t->simbolo);
        printf("ERRO analisa_fator: entrada invalida\n");
        exit(1);
    }
}

void analisa_termo(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    analisa_fator(t, arquivo, fila, caractere);
    while(strcmp(t->simbolo, "smult") == 0 || strcmp(t->simbolo, "sdiv") == 0 || strcmp(t->simbolo, "se") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_fator(t, arquivo, fila, caractere);
    }
}

void analisa_expressao_simples(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "smais") == 0 || strcmp(t->simbolo, "smenos") == 0) {
        lexico(t, arquivo, fila, caractere);
    }
    analisa_termo(t, arquivo, fila, caractere);
    while(strcmp(t->simbolo, "smais") == 0 || strcmp(t->simbolo, "smenos") == 0 || strcmp(t->simbolo, "sou") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_termo(t, arquivo, fila, caractere);
    }

}

void analisa_comandos(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "sinicio") == 0) {
        lexico(t, arquivo, fila, caractere);
        analisa_comando_simples(t, arquivo, fila, caractere);
        while(strcmp(t->simbolo, "sfim") != 0) {
            if(strcmp(t->simbolo, "sponto_virgula") == 0) {
                lexico(t, arquivo, fila, caractere);
                if(strcmp(t->simbolo, "sfim") != 0) {
                    analisa_comando_simples(t, arquivo, fila, caractere);
                }
            }
            else {
                printf("ERRO analisa_comandos: esperado <;>\n");
                exit(1);
            }

        }
        lexico(t, arquivo, fila, caractere);
    }
    else {
        printf("ERRO analisa_comandos: esperado sinicio\n");
    }
}

void analisa_comando_simples(token *t, FILE *arquivo, fila_tokens *fila, int* caractere) {
    if(strcmp(t->simbolo, "sidentificador") == 0) {
        analisa_atrib_chprocedimento(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "sse") == 0) {
        analisa_se(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "senquanto") == 0) {
        analisa_enquanto(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "sleia") == 0) {
        analisa_leia(t, arquivo, fila, caractere);
    }
    else if(strcmp(t->simbolo, "sescreva") == 0) {
        analisa_escreva(t, arquivo, fila, caractere);
    }
    else {
        analisa_comandos(t, arquivo, fila, caractere);
    }
}

int main() {
    token t;
    FILE *entrada = NULL;
    fila_tokens fila = {NULL, NULL};

    entrada = fopen("sint13.txt", "r");

    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }
    int caractere;
    caractere = fgetc(entrada);

    int teste = 0;
    lexico(&t, entrada, &fila, &caractere);

    if(strcmp(t.simbolo, "sprograma") == 0){
        lexico(&t, entrada, &fila, &caractere);
        if(strcmp(t.simbolo, "sidentificador") == 0){
            lexico(&t, entrada, &fila, &caractere);
            if(strcmp(t.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(&t, entrada, &fila, &caractere);
                if(strcmp(t.simbolo, "sponto") == 0) {
                    if(lexico(&t, entrada, &fila, &caractere) == 2) {
                        printf(" Sucesso!");
                    }
                    else {
                        printf("ERRO");
                    }
                }
                else {
                    printf("ERRO programa: esperado <.>\n");
                }
            }
            else {
                printf("ERRO programa: esperado <;>");
            }
        }
        else{
            printf("ERRO programa: identificador nao encontrado\n");
        }
    }
    else  {
        printf("ERRO programa: sprograma nao encontrado\n");
    }

    while(teste != 2){
        teste = lexico(&t, entrada, &fila, &caractere);
    }

    fclose(entrada);
    imprimir_lista_tokens(&fila);

    return 0;
}
