#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexico.h"


void analisa_comandos(fila_tokens *fila);
void analisa_comando_simples(fila_tokens *fila);
void analisa_expressao_simples(fila_tokens *fila);
void analisa_termo(fila_tokens *fila);
void analisa_fator(fila_tokens *fila);
void analisa_declaracao_procedimento(fila_tokens *fila);
void analisa_subrotinas(fila_tokens *fila);
void analisa_bloco(fila_tokens *fila);


void analisa_chamada_funcao(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "sidentificador") != 0) {
        printf("ERRO analisa_chamada_funcao: esperado identificador\n");
    }
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

void analisa_tipo(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "sinteiro") != 0 && strcmp(token_atual.simbolo, "sbooleano") != 0) {
        printf("ERRO analisa_tipo: tipo de variavel invalida\n");
    }
    lexico(fila);
}

void analisa_leia(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                lexico(fila);
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

void analisa_escreva(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                lexico(fila);
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

void analisa_variaveis(fila_tokens *fila) {
    while(strcmp(token_atual.simbolo, "sdoispontos") != 0) {
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
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
            printf("ERRO analisa_variaveis: esperado identificador\n");
        }
    }
    lexico(fila);
    analisa_tipo(fila);
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
    lexico(fila);
    analisa_expressao(fila);
    if(strcmp(token_atual.simbolo, "sfaca") == 0) {
        lexico(fila);
        analisa_comando_simples(fila);
    }
    else {
        printf("ERRO analisa_enquanto: esperado <faca>\n");
    }
}

void analisa_declaracao_procedimento(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
            analisa_bloco(fila);
        }
        else {
            printf("ERRO analisa_declaracao_procedimento: esperado <;>\n");
        }
    }
    else {
        printf("ERRO analisa_declaracao_procedimento: esperado identificador\n");
    }
}

void analisa_declaracao_funcao(fila_tokens *fila) {
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sdoispontos") == 0) {
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sinteiro") == 0 || strcmp(token_atual.simbolo, "sbooleano") == 0) {
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
    else {
        printf("ERRO analisa_declaracao_funcao: esperado identificador\n");
    }
}

void analisa_subrotinas(fila_tokens *fila) {
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
}

void analisa_bloco(fila_tokens *fila) {
    lexico(fila);
    analisa_et_variaveis(fila);
    analisa_subrotinas(fila);
    analisa_comandos(fila);
}

void analisa_fator(fila_tokens *fila) {
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        analisa_chamada_funcao(fila);
        lexico(fila);
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

    arquivo = fopen("codigo_teste.txt", "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    caractere = fgetc(arquivo);

    int teste = 0;
    lexico(&fila);

    if(strcmp(token_atual.simbolo, "sprograma") == 0){
        lexico(&fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0){
            lexico(&fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(&fila);
                if(strcmp(token_atual.simbolo, "sponto") == 0) {
                    if(lexico(&fila) == 2) {
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
    else {
        printf("ERRO programa: sprograma nao encontrado\n");
    }

    while(teste != 2){
        teste = lexico(&fila);
    }

    fclose(arquivo);
    imprimir_lista_tokens(&fila);

    return 0;
}
