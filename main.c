#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexico.h"

typedef struct {
    char lexema[50];
    char escopo[2];
    char tipo[50];
    int endereco;
}simbolo;

typedef struct {
    simbolo *simbolos;
    int tamanho;
}tabela_simbolos;

tabela_simbolos tSimb = {NULL, 0};
int endereco_global = 0;
int rotulo = 0;

void analisa_comandos(fila_tokens *fila);
void analisa_comando_simples(fila_tokens *fila);
void analisa_expressao_simples(fila_tokens *fila);
void analisa_termo(fila_tokens *fila);
void analisa_fator(fila_tokens *fila);
void analisa_declaracao_procedimento(fila_tokens *fila);
void analisa_subrotinas(fila_tokens *fila);
void analisa_bloco(fila_tokens *fila);

void gera(int rot, const char *arg1, int arg2, const char *arg3) {
    if (arquivo_obj == NULL) {
        printf("Erro: arquivo .obj nao foi aberto\n");
        return;
    }

    fprintf(arquivo_obj, "%d %s %d %s\n", rot, arg1, arg2, arg3);
}


/*
char converte_posfixa(char expressao) { //expressao = x-3

    int tam = strlen(expressao);
    for(int i = 0; i < tam; i++) {
        if(isalpha(expressao[i]) || isdigit(expressao[i])) {
           // expr_pos := expr_pos + expressao[i]
        }
        else if(strcmp(expressao[i], '(') == 0) {
            //empilha '('
        }
        else if(strcmp(expressao[i], ')') == 0) {
            //desempilha
            while(strcmp(expressao[i], '(') != 0) {
                //desempilha tudo
            }
        }
        else if(strcmp(expressao[i], ''))
    }

    return expr_pos;
}
*/

void insere_tabela(const char *lexema, const char *tipo, const char *escopo, int rot) {
    tSimb.tamanho++;

    simbolo *temp = realloc(tSimb.simbolos, tSimb.tamanho * sizeof(simbolo));
    if(temp == NULL){
        printf("Erro na alocação de memória\n");
        free(tSimb.simbolos);
        exit(1);
    }

    tSimb.simbolos = temp;

    simbolo *novo = &tSimb.simbolos[tSimb.tamanho - 1];
    strcpy(novo->lexema, token_atual.lexema);
    strcpy(novo->tipo, tipo);
    strcpy(novo->escopo, escopo);
    novo->endereco = endereco_global;
    endereco_global++;
}

void remove_tabela() {
    int i = tSimb.tamanho - 1;
    if(tSimb.tamanho > 0) {
        while((strcmp(tSimb.simbolos[i].escopo, "L") != 0) && tSimb.tamanho > 0) {
            tSimb.tamanho--;
            simbolo *temp = realloc(tSimb.simbolos, tSimb.tamanho * sizeof(simbolo));
            if(temp != NULL || tSimb.tamanho == 0) {
                tSimb.simbolos = temp;
            }
            i--;
        }
    }
    else {
        printf("Tabela de simbolos esta vazia\n");
    }
}

void imprime_tabela() {
    printf("\n--- Tabela de Símbolos ---\n");
    for (int i = 0; i < tSimb.tamanho; i++) {
        printf("Lexema: %-10s | Tipo: %-10s | Escopo: %s | Endereco: %d\n",
               tSimb.simbolos[i].lexema,
               tSimb.simbolos[i].tipo,
               tSimb.simbolos[i].escopo,
               tSimb.simbolos[i].endereco);
    }
}

int pesquisa_duplicvar_tabela(const char *lexema) {
    for(int i = tSimb.tamanho; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            return 1;
        }
        if(strcmp(tSimb.simbolos[i].escopo, "L")){
            return 0;
        }
    }
    return 0;
}

int pesquisa_duplicfunc_tabela(const char *lexema) {
    for(int i = 0; i < tSimb.tamanho; i++) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            return 0;
        }
    }

    return 1;
}

int pesquisa_declvarfunc_tabela(const char * lexema) {
    for(int i = 0; i < tSimb.tamanho; i++) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "inteiro") || strcmp(tSimb.simbolos[i].tipo, "booleano")){
                return 1;
            }
            if(strcmp(tSimb.simbolos[i].tipo, "funcao_inteiro") || strcmp(tSimb.simbolos[i].tipo, "funcao_booleano")){
                return 1;
            }
            printf("ERRO: variavel ou funcao nao declarada");
        }
    }

    return 0;
}

void coloca_tipo_tabela(const char *lexema, int qtdVar) {
        if(strcmp(token_atual.lexema, "inteiro") == 0) {
            for(int i = 0; i < qtdVar; i++) {
                strcpy(tSimb.simbolos[tSimb.tamanho -i -1].tipo, "inteiro");
            }
        }
        else if(strcmp(token_atual.lexema, "booleano") == 0) {
            for(int i = 0; i < qtdVar; i++) {
                strcpy(tSimb.simbolos[tSimb.tamanho -i -1].tipo, "booleano");
            }
        }
}

int pesquisa_declvar_tabela(const char *lexema) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "inteiro") || strcmp(tSimb.simbolos[i].tipo, "booleano")){
                return i;
            }
            printf("ERRO: variavel nao declarada");
        }
    }

    return -1;
}

int pesquisa_declproc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro: ja existe uma variavel/funcao/procedimento com o nome %s\n", token_atual.lexema);
            printf("%s", tSimb.simbolos[i].lexema);
            return 0;
        }
    }

    return 1;
}

int pesquisa_tabela(const char *lexema, char nivel, int *ind) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            ind = i;
            return 1;
        }
    }

    return 0;
}

int pesquisa_declfunc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro: ja existe uma variavel/funcao/procedimento com o nome %s\n", token_atual.lexema);
            printf("%s", tSimb.simbolos[i].lexema);
            return 0;
        }
    }

    return 1;
}

void analisa_chamada_funcao(fila_tokens *fila) {

    printf("oi sou o %s\n", token_atual.lexema);
    lexico(fila);
}

void analisa_chamada_procedimento(fila_tokens *fila) {
    /*
    if(strcmp(token_atual.simbolo, "sidentificador") != 0) {
       printf("ERRO analisa_chamado_procedimento: esperado identificador\n");
    }
    */
}

void analisa_expressao(fila_tokens *fila) {
    analisa_expressao_simples(fila);
    if(strcmp(token_atual.simbolo, "smaior") == 0 || strcmp(token_atual.simbolo, "smaiorig") == 0 ||
       strcmp(token_atual.simbolo, "sig") == 0 || strcmp(token_atual.simbolo, "smenor") == 0 ||
       strcmp(token_atual.simbolo, "smenorig") == 0 || strcmp(token_atual.simbolo, "sdif") == 0) {
        lexico(fila);
        analisa_expressao_simples(fila);
    }
}

void analisa_atribuicao(fila_tokens *fila) {
    lexico(fila);
    analisa_expressao(fila);
}

void analisa_tipo(fila_tokens *fila, int qtdVar) {
    if(strcmp(token_atual.simbolo, "sinteiro") != 0 && strcmp(token_atual.simbolo, "sbooleano") != 0) {
        printf("ERRO analisa_tipo: tipo de variavel invalida\n");
    }
    else {
        coloca_tipo_tabela(token_atual.lexema, qtdVar);
        lexico(fila);
    }
}

void analisa_leia(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            int index = pesquisa_declvar_tabela(token_atual.lexema);
            if(index != -1 && strcmp(tSimb.simbolos[index].tipo, "inteiro") == 0) {
                    lexico(fila);
                    if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                        lexico(fila);
                    }
                    else {
                        printf("ERRO analisa_leia: esperado <)>\n");
                    }
            }
            else if(index != -1){
                printf("lexema atual: %s", token_atual.lexema);
                printf("Erro: leia nao aceita variavel do tipo %s\n", tSimb.simbolos[index].tipo);
            }
            else {
                printf("ERRO variavel %s nao foi declarada\n", token_atual.lexema);
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

void analisa_escreva(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            int index = pesquisa_declvar_tabela(token_atual.lexema);
            if(index != -1 && strcmp(tSimb.simbolos[index].tipo, "inteiro") == 0) {
                lexico(fila);
                if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                    lexico(fila);
                }
                else {
                    printf("ERRO analisa_escreva: esperado <)>\n");
                }
            }
            else if(index != -1){
                printf("Erro: escreva nao aceita variavel do tipo %s\n", tSimb.simbolos[index].tipo);
            }
            else {
                printf("ERRO: variavel %s nao foi declarada\n", token_atual.lexema);
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

void analisa_variaveis(fila_tokens *fila) {
    int qtdVar = 0;
    while(strcmp(token_atual.simbolo, "sdoispontos") != 0) {
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            qtdVar++;
            if(!pesquisa_duplicvar_tabela(token_atual.lexema)){
                insere_tabela(token_atual.lexema, "variavel", "", 0);
                lexico(fila);
                if(strcmp(token_atual.simbolo, "svirgula") == 0 || strcmp(token_atual.simbolo, "sdoispontos") == 0) {
                    if(strcmp(token_atual.simbolo, "svirgula") == 0) {
                        lexico(fila);
                        if(strcmp(token_atual.simbolo, "sdoispontos") == 0) {
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
                printf("ERRO: variavel com o nome %s ja existe", token_atual.lexema);
            }
        }
        else {
            printf("ERRO analisa_variaveis: esperado identificador\n");
        }
    }
    lexico(fila);
    analisa_tipo(fila, qtdVar);
}

void analisa_et_variaveis(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "svar") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            while(strcmp(token_atual.simbolo, "sidentificador") == 0) {
                analisa_variaveis(fila);
                if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                    lexico(fila);
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

void analisa_atrib_chprocedimento(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "satribuicao") == 0) {
        analisa_atribuicao(fila);
    }
    else {
        analisa_chamada_procedimento(fila);
    }
}

void analisa_se(fila_tokens *fila) {
    lexico(fila);
    analisa_expressao(fila);
    if(strcmp(token_atual.simbolo, "sentao") == 0) {
        lexico(fila);
        analisa_comando_simples(fila);
        if(strcmp(token_atual.simbolo, "ssenao") == 0) {
            lexico(fila);
            analisa_comando_simples(fila);
        }
    }
    else {
        printf("ERRO analisa_se: esperado <entao>\n");
    }
}

void analisa_enquanto(fila_tokens *fila) {
    int auxrot1, auxrot2;

    auxrot1 = rotulo;
    gera(rotulo, NULL, "", "");
    rotulo++;

    lexico(fila);
    analisa_expressao(fila);
    if(strcmp(token_atual.simbolo, "sfaca") == 0) {
        auxrot2 = rotulo;
        gera("", "JMPF", rotulo, "");
        rotulo++;
        lexico(fila);
        analisa_comando_simples(fila);
        gera("", "JMP", auxrot1, "");
        gera(auxrot2, NULL, "", "");
    }
    else {
        printf("ERRO analisa_enquanto: esperado <faca>\n");
    }
}

void analisa_declaracao_procedimento(fila_tokens *fila) {
    lexico(fila);
    char nivel[2];
    strcpy(nivel, "L");
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        if(pesquisa_declproc_tabela()) {
            insere_tabela(token_atual.lexema, "procedimento", nivel, rotulo);
            gera(rotulo, "NULL", "", "");
            rotulo++;
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(fila);
            }
            else {
                printf("ERRO analisa_declaracao_procedimento: esperado <;>\n");
            }
        }
        else {
            printf("ERRO: Procedimento ja foi declarado\n");
        }
    }
    else {
        printf("ERRO analisa_declaracao_procedimento: esperado identificador\n");
    }
    remove_tabela();
}

void analisa_declaracao_funcao(fila_tokens *fila) {
    lexico(fila);
    char nivel[2];
    strcpy(nivel, "L");

    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        if(pesquisa_declfunc_tabela(token_atual.lexema)) {
            insere_tabela(token_atual.lexema, "", nivel, rotulo);
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sdoispontos") == 0) {
                lexico(fila);
                if(strcmp(token_atual.simbolo, "sinteiro") == 0 || strcmp(token_atual.simbolo, "sbooleano") == 0) {
                    if(strcmp(token_atual.simbolo, "sinteiro") == 0) {
                        strcpy(tSimb.simbolos[tSimb.tamanho -1].tipo, "funcao_inteiro");
                    }
                    else {
                        strcpy(tSimb.simbolos[tSimb.tamanho -1].tipo, "funcao_booleano");
                    }
                    lexico(fila);
                    if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                        analisa_bloco(fila);
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
    }
    else {
        printf("ERRO analisa_declaracao_funcao: esperado identificador\n");
    }
    remove_tabela();
}

void analisa_subrotinas(fila_tokens *fila) {

    int auxrot;
    int flag = 0;

    if(strcmp(token_atual.simbolo, "sprocedimento") == 0 || strcmp(token_atual.simbolo, "sfuncao") == 0) {
        auxrot = rotulo;
        gera("", "JMP", rotulo, "");
        rotulo++;
        flag = 1;
    }

    while(strcmp(token_atual.simbolo, "sprocedimento") == 0 || strcmp(token_atual.simbolo, "sfuncao") == 0) {
        if(strcmp(token_atual.simbolo, "sprocedimento") == 0) {
            analisa_declaracao_procedimento(fila);
        }
        else {
            analisa_declaracao_funcao(fila);
        }

        if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
            lexico(fila);
        }
        else {
            printf("ERRO analisa_subrotinas: esperado <;>\n");
        }
    }

    if(flag == 1) {
        gera(auxrot, "NULL", "", "");
    }
}

void analisa_bloco(fila_tokens *fila) {
    lexico(fila);
    analisa_et_variaveis(fila);
    analisa_subrotinas(fila);
    analisa_comandos(fila);
}

void analisa_fator(fila_tokens *fila) {
    int ind = 0;
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        printf("%s", token_atual.lexema);
        if(pesquisa_tabela(token_atual.lexema, "L", &ind)) {
                if((strcmp(tSimb.simbolos[ind].tipo, "funcao_inteiro") == 0) || (strcmp(tSimb.simbolos[ind].tipo, "funcao_booleano") == 0)) {
                    analisa_chamada_funcao(fila);
                }
                else {
                    lexico(fila);
                }
        }
        else {
            printf("Erro: identificador nao encontrado\n");
        }
    }
    else if(strcmp(token_atual.simbolo, "snumero") == 0) {
        lexico(fila);
    }
    else if(strcmp(token_atual.simbolo, "snao") == 0) {
        lexico(fila);
        analisa_fator(fila);
    }
    else if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        analisa_expressao(fila);
        if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
            lexico(fila);
        }
        else {
            printf("ERRO analisa_fator: esperado <)>\n");
        }
    }
    else if(strcmp(token_atual.lexema, "verdadeiro") == 0 || strcmp(token_atual.lexema, "falso") == 0) {
        lexico(fila);
    }
    else {
        printf("%s", token_atual.simbolo);
        printf("ERRO analisa_fator: entrada invalida\n");
        exit(1);
    }
}

void analisa_termo(fila_tokens *fila) {
    analisa_fator(fila);
    while(strcmp(token_atual.simbolo, "smult") == 0 || strcmp(token_atual.simbolo, "sdiv") == 0 || strcmp(token_atual.simbolo, "se") == 0) {
        lexico(fila);
        analisa_fator(fila);
    }
}

void analisa_expressao_simples(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0) {
        lexico(fila);
    }
    analisa_termo(fila);
    while(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0 || strcmp(token_atual.simbolo, "sou") == 0) {
        lexico(fila);
        analisa_termo(fila);
    }

}

void analisa_comandos(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "sinicio") == 0) {
        lexico(fila);
        analisa_comando_simples(fila);
        while(strcmp(token_atual.simbolo, "sfim") != 0) {
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                lexico(fila);
                if(strcmp(token_atual.simbolo, "sfim") != 0) {
                    analisa_comando_simples(fila);
                }
            }
            else {
                printf("ERRO analisa_comandos: esperado <;>\n");
                exit(1);
            }

        }
        lexico(fila);
    }
    else {
        printf("ERRO analisa_comandos: esperado sinicio\n");
        printf("%s", token_atual.lexema);
    }
}

void analisa_comando_simples(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        analisa_atrib_chprocedimento(fila);
    }
    else if(strcmp(token_atual.simbolo, "sse") == 0) {
        analisa_se(fila);
    }
    else if(strcmp(token_atual.simbolo, "senquanto") == 0) {
        analisa_enquanto(fila);
    }
    else if(strcmp(token_atual.simbolo, "sleia") == 0) {
        analisa_leia(fila);
    }
    else if(strcmp(token_atual.simbolo, "sescreva") == 0) {
        analisa_escreva(fila);
    }
    else {
        analisa_comandos(fila);
    }
}

int main() {

    fila_tokens fila = {NULL, NULL};

    arquivo_obj = fopen("arquivo.obj", "w"); //arquivo obj

    if(arquivo_obj == NULL) {
        printf("Erro ao criar arquivo\n");
        exit(1);
    }

    arquivo = fopen("exslide70.txt", "r");

    if(arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    caractere = fgetc(arquivo);

    int teste = 0;
    lexico(&fila);

    rotulo = 1;

    if(strcmp(token_atual.simbolo, "sprograma") == 0){
        lexico(&fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0){
            insere_tabela(token_atual.lexema, "nomedeprograma", "", 0);
            lexico(&fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(&fila);
                if(strcmp(token_atual.simbolo, "sponto") == 0) {
                    if(lexico(&fila) == 2) {
                        printf(" Sucesso!");
                        gera("oi", "oi", "oi", "oi");
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
            //erro duplicvar
        }
        else{
            printf("ERRO programa: identificador nao encontrado\n");
        }
    }
    else {
        printf("ERRO programa: sprograma nao encontrado\n");
    }

    while(teste != 2){
        teste = lexico(&fila);
    }

    if(arquivo_obj != NULL) {
        fclose(arquivo_obj);
        arquivo_obj = NULL;
    }

    fclose(arquivo);
    imprimir_lista_tokens(&fila);
    imprime_tabela();
    return 0;
}
