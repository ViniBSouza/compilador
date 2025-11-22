#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexico.h"
#include "estruturas.h"

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
int endereco_tabela = 0;
int endereco_mvd = 1;
int rotulo = 0;

void analisa_comandos();
void analisa_comando_simples();
void analisa_expressao_simples(ListaOperadores *lista,Pilha *pilhapos);
void analisa_termo(ListaOperadores *lista,Pilha *pilhapos);
void analisa_fator( ListaOperadores *lista,Pilha *pilhapos);
void analisa_declaracao_procedimento();
void analisa_subrotinas();
void analisa_bloco();

void gera(int rot, const char *arg1, int arg2, int arg3) {
    if (arquivo_obj == NULL) {
        printf("Erro: arquivo .obj nao foi aberto\n");
        return;
    }

    // ---- rot (4 colunas) ----
    if (rot >= 0)
        fprintf(arquivo_obj, "%-4d", rot);   // alinhado à esquerda, preenchido com espaços
    else
        fprintf(arquivo_obj, "    ");        // 4 espaços

    // ---- arg1 (8 colunas) ----
    fprintf(arquivo_obj, "%-8s", arg1);      // string preenchida até 8 chars

    // ---- arg2 (4 colunas) ----
    if (arg2 >= 0)
        fprintf(arquivo_obj, "%-4d", arg2);
    else
        fprintf(arquivo_obj, "    ");

    // ---- arg3 (4 colunas + quebra de linha) ----
    if (arg3 >= 0)
        fprintf(arquivo_obj, "%-4d\n", arg3);
    else
        fprintf(arquivo_obj, "    \n");
}

void insere_tabela(const char *lexema, const char *tipo, const char *escopo, int rot) {
    tSimb.tamanho++;

    simbolo *temp = realloc(tSimb.simbolos, tSimb.tamanho * sizeof(simbolo));
    if(temp == NULL){
        printf("Erro na alocacao de mem�ria\n");
        free(tSimb.simbolos);
        exit(1);
    }

    tSimb.simbolos = temp;

    simbolo *novo = &tSimb.simbolos[tSimb.tamanho - 1];
    strcpy(novo->lexema, token_atual.lexema);
    strcpy(novo->tipo, tipo);
    strcpy(novo->escopo, escopo);
    if(strcmp(tipo,"-") == 0  || strcmp(tipo,"procedimento") == 0){
        novo->endereco = rot;

    }else{
        novo->endereco = endereco_tabela;
        endereco_tabela++;
    }
}

void imprime_tabela() {
    printf("\n--- Tabela de S�mbolos ---\n");
    for (int i = 0; i < tSimb.tamanho; i++) {
        printf("Lexema: %-10s | Tipo: %-10s | Escopo: %s | Endereco: %d\n",
               tSimb.simbolos[i].lexema,
               tSimb.simbolos[i].tipo,
               tSimb.simbolos[i].escopo,
               tSimb.simbolos[i].endereco);
    }
}

void remove_tabela(char *identificador) {

    int i = 0;


    while (i < tSimb.tamanho && strcmp(identificador, tSimb.simbolos[i].lexema) != 0) {
        i++;
    }
    i++;

    int inicio = i;

    while (i < tSimb.tamanho && strcmp(tSimb.simbolos[i].escopo, "L") != 0) {
        i++;
    }

    int fim = i; // posição onde para a remoção
    int count = fim - inicio;

    if (count > 0) {
        int enderecoinicial = tSimb.simbolos[inicio].endereco;
        for (int j = fim; j < tSimb.tamanho; j++) {
            tSimb.simbolos[j - count] = tSimb.simbolos[j];
        }
        tSimb.tamanho -= count;
        tSimb.simbolos = realloc(tSimb.simbolos, tSimb.tamanho * sizeof(simbolo));

        gera(-1, "DALLOC", enderecoinicial, count);
    }

    //imprime_tabela();
}




int pesquisa_duplicvar_tabela(const char *lexema) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(tSimb.simbolos[i].escopo, "L") == 0){
            return 0;
        }
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            return 1;
        }
    }
    return 0;
}

int pesquisa_duplicfunc_tabela(const char *lexema) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            return 0;
        }
    }

    return 1;
}

int pesquisa_declvarfunc_tabela(const char * lexema) {
    for(int i = 0; i < tSimb.tamanho - 1; i++) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "inteiro") || strcmp(tSimb.simbolos[i].tipo, "booleano")){
                return 1;
            }
            if(strcmp(tSimb.simbolos[i].tipo, "funcao_inteiro") || strcmp(tSimb.simbolos[i].tipo, "funcao_booleano")){
                return 1;
            }
            printf("ERRO Linha %d: variavel ou funcao %s nao declarada",linha,lexema);
            exit(1);
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
        if(strcmp(lexema, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "inteiro") || strcmp(tSimb.simbolos[i].tipo, "booleano")){
                return i;
            }
            printf("ERRO %d: variavel %s nao declarada",linha,lexema);
            exit(1);
        }
    }

    return -1;
}

int pesquisa_declproc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro Linha %d: nao e possivel criar um procedimento como o nome %s pois ja existe uma variavel/funcao/procedimento\n",linha, token_atual.lexema);
            //printf("%s", tSimb.simbolos[i].lexema);
            exit(1);
            return 0;
        }
    }

    return 1;
}

int pesquisa_tabela(const char *lexema, int *ind) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(lexema, tSimb.simbolos[i].lexema) == 0) {
            *ind = i;
            return 1;
        }
    }

    return 0;
}

int encontra_func(const char *lexema,int *numVariavel, int *primeiroEndereco){
    int i = tSimb.tamanho-1;
    *numVariavel = 0;
    while (i >= 0 &&
       (
        strcmp(tSimb.simbolos[i].lexema, lexema) != 0 ||
        (
         strcmp(tSimb.simbolos[i].tipo, "funcao_inteiro") != 0 &&
         strcmp(tSimb.simbolos[i].tipo, "funcao_booleano") != 0
        )
       ))
        {
            i--;
        }
    int localizacaofuncao = i;
    i++;

    *primeiroEndereco = tSimb.simbolos[i].endereco;
    while(i < tSimb.tamanho && strcmp(tSimb.simbolos[i].escopo,"L" ) != 0){
       (*numVariavel)++;
        i++;
    }
    if(i > 0){
        return localizacaofuncao;
    }
    return -1;

}

int pesquisa_declfunc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro Linha %d: ja existe uma variavel/funcao/procedimento com o nome %s\n", linha,token_atual.lexema);
            exit(1);
            //printf("%s", tSimb.simbolos[i].lexema);
            return 0;
        }
    }

    return 1;
}

//fazer
void analisa_chamada_funcao() {
    /* pesquisa_declvarfunc_tabela(token_atual.lexema);
    printf("oi sou o %s\n", token_atual.lexema);
    lexico(fila);
    */
}

//fazer
int analisa_chamada_procedimento(char *ident_proc) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(ident_proc, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "procedimento") == 0) {
                //printf("%s", tSimb.simbolos[i].lexema);
                gera(-1,"CALL",tSimb.simbolos[i].endereco,-1);
                return 0; //verdadeiro: proc existe
            }
            else {
                return 1;
            }
        }
    }
    printf("Erro Linha %d: procedimento %s nao existe",linha, ident_proc);
    exit(1);
    return 1;
}

int precedenciaGera(char* operador){

    if((strcmp(operador, "sou") == 0)){
        gera(-1,"OR",-1,-1);
        return 0;
    }else if((strcmp(operador, "se") == 0)){
        gera(-1,"AND",-1,-1);
        return 1;
    }else if((strcmp(operador, "snao") == 0)){
        gera(-1,"NEG",-1,-1);
        return 2;
    }else if((strcmp(operador, "sdif") == 0)){
        gera(-1,"CDIF",-1,-1);
        return 4;
    }else if((strcmp(operador, "sig") == 0)){
        gera(-1,"CEQ",-1,-1);
        return 4;
    }else if((strcmp(operador, "smenorig") == 0)){
        gera(-1,"CMEQ",-1,-1);
        return 4;
    }else if((strcmp(operador, "smenor") == 0)){
        gera(-1,"CME",-1,-1);
        return 4;
    }else if((strcmp(operador, "smaiorig") == 0)){
        gera(-1,"CMAQ",-1,-1);
        return 4;
    }else if((strcmp(operador, "smaior") == 0)){
        gera(-1,"CMA",-1,-1);
        return 4;
    }else if((strcmp(operador, "smais") == 0)){
        gera(-1,"ADD",-1,-1);
        return 5;
    }else if((strcmp(operador, "smenos") == 0)){
        gera(-1,"SUB",-1,-1);
        return 5;
    }else if((strcmp(operador, "sdiv") == 0)){
        gera(-1,"DIVI",-1,-1);
        return 6;
    }else if((strcmp(operador, "smult") == 0)){
        gera(-1,"MULT",-1,-1);
        return 6;
    }else if((strcmp(operador, "#") == 0)){
        gera(-1,"INV",-1,-1);
        return 7;
    }else if((strcmp(operador, "@") == 0)){
        return 7;
    }

    return 8;
}

int precedencia(char* operador){
    if((strcmp(operador, "sou") == 0)){
        return 0;
    }else if((strcmp(operador, "se") == 0)){
        return 1;
    }else if((strcmp(operador, "snao") == 0)){
        return 2;
    }else if((strcmp(operador, "sdif") == 0)){
        return 4;
    }else if((strcmp(operador, "smenorig") == 0)){
        return 4;
    }else if((strcmp(operador, "smenor") == 0)){
        return 4;
    }else if((strcmp(operador, "smaiorig") == 0)){
        return 4;
    }else if((strcmp(operador, "smaior") == 0)){
        return 4;
    }else if((strcmp(operador, "sig") == 0)){
        return 4;
    }
    else if((strcmp(operador, "smais") == 0)){
        return 5;
    }else if((strcmp(operador, "smenos") == 0)){
        return 5;
    }
    else if((strcmp(operador, "sdiv") == 0)){
        return 6;
    }else if((strcmp(operador, "smult") == 0)){
        return 6;
    }else if((strcmp(operador, "#") == 0)){
        return 7;
    }else if((strcmp(operador, "@") == 0)){
        return 7;
    }

    return 8;
}
int verificaPrecedencia(char* operador1, char* operador2){

    if(precedencia(operador1) <= precedencia(operador2)){
        return 1;
    }

	return 0;
}

void trataOperadorPos(ListaOperadores* lista, Pilha* pilhapos){
    NoOperador*aux = pilhapos->topo;
	while(pilhapos->topo != NULL && strcmp(aux->t.simbolo, "sabre_parenteses") != 0 && verificaPrecedencia(token_atual.simbolo, aux->t.simbolo)){
		insere_lista(pop(pilhapos),lista);
		aux = pilhapos->topo;
	}
	push(pilhapos,token_atual);

}

ListaOperadores analisa_expressao() {
    Pilha * pilhapos = criaPilha();
    ListaOperadores lista;
    lista.operadores = NULL;
    lista.tamanho = 0;
    analisa_expressao_simples(&lista,pilhapos);
    if(strcmp(token_atual.simbolo, "smaior") == 0 || strcmp(token_atual.simbolo, "smaiorig") == 0 ||
       strcmp(token_atual.simbolo, "sig") == 0 || strcmp(token_atual.simbolo, "smenor") == 0 ||
       strcmp(token_atual.simbolo, "smenorig") == 0 || strcmp(token_atual.simbolo, "sdif") == 0) {
        trataOperadorPos(&lista,pilhapos);
        lexico();
        analisa_expressao_simples(&lista,pilhapos);
    }

    while(pilhapos->topo != NULL){
        insere_lista(pop(pilhapos),&lista);
    }

    /*
    printf("\nOLHA SO A MINHA EXPRESSAO POSFIXE EBAAAA: \n");
    for(int i = 0; i< lista.tamanho;i++){
        printf("%s",lista.operadores[i].simbolo);
    }
    */
    while (pilhapos->topo != NULL){
        pop(pilhapos);  // que deve liberar os nós
    }
    free(pilhapos);
    return lista;
}

void verifica_tipo_variavel_funcao(PilhaTipo *pilhaT,char* lexema){
    int index;
    pesquisa_tabela(lexema,&index);
    if(index == 0){
        printf("ERRO Linha %d: variavel/funcao nao declarada",linha);
        exit(1);
    }

    if(strcmp(tSimb.simbolos[index].tipo,"nomedeprograma") == 0 || strcmp(tSimb.simbolos[index].tipo,"procedimento") == 0){
        printf("ERRO Linha: %d: %s nao eh variavel/funcao",linha, lexema);
        exit(1);
    }
    if(strcmp(tSimb.simbolos[index].tipo, "funcao_inteiro") == 0) {
        pushTipo(pilhaT,"inteiro");
        gera(-1,"CALL",tSimb.simbolos[index].endereco,-1);
    }else if(strcmp(tSimb.simbolos[index].tipo,"inteiro") == 0){
        pushTipo(pilhaT,"inteiro");
        gera(-1,"LDV",tSimb.simbolos[index].endereco,-1);
    }else if(strcmp(tSimb.simbolos[index].tipo,"funcao_booleano") == 0){
        pushTipo(pilhaT,"booleano");
        gera(-1,"CALL",tSimb.simbolos[index].endereco,-1);
    }
    else{
        pushTipo(pilhaT,"booleano");
        gera(-1,"LDV",tSimb.simbolos[index].endereco,-1);
    }

}

char* analisa_tipo_expressao(ListaOperadores lista){
    PilhaTipo* pilhaT = criaPilhaTipo();
    char* tipo;
    int j;
    int precedenciaOperador;
    for(int i = 0; i< lista.tamanho;i++){
        precedenciaOperador = precedenciaGera(lista.operadores[i].simbolo);
        if(strcmp(lista.operadores[i].simbolo, "sidentificador") == 0){
            verifica_tipo_variavel_funcao(pilhaT, lista.operadores[i].lexema);
        }else if(strcmp(lista.operadores[i].simbolo, "sverdadeiro") == 0){
            pushTipo(pilhaT,"booleano");
            gera(-1,"LDC",1,-1);
        }else if(strcmp(lista.operadores[i].simbolo, "sfalso") == 0){
            pushTipo(pilhaT,"booleano");
            gera(-1,"LDC",0,-1);
        }else if(strcasecmp(lista.operadores[i].simbolo,"snumero") == 0){
            pushTipo(pilhaT,"inteiro");
            gera(-1,"LDC",atoi(lista.operadores[i].lexema),-1);
        }else if(precedenciaOperador >= 4 && precedenciaOperador < 7 ){
            for(j = 0; j< 2; j++){
                tipo = popTipo(pilhaT);
                if(strcmp(tipo,"inteiro") != 0){
                    printf("ERRO Linha %d: esperado dois inteiros na expressao para o sinal %s ",linha,lista.operadores[i].lexema);
                    exit(1);
                }
            }
                if(precedenciaOperador == 4){
                    pushTipo(pilhaT,"booleano");
                }else{
                    pushTipo(pilhaT,"inteiro");
                }
        }
        else if(precedenciaOperador < 2){
            for(j = 0; j< 2; j++){
                tipo = popTipo(pilhaT);
                if(strcmp(tipo,"booleano") != 0){
                    printf("ERRO Linha %d: esperado dois booleanos na expressao para o sinal %s ",linha,lista.operadores[i].lexema);
                    exit(1);
                }
            }
            pushTipo(pilhaT,"booleano");
        }else if(precedenciaOperador == 7){
            tipo = popTipo(pilhaT);
            if(strcmp(tipo,"inteiro") != 0 ){
                printf("ERRO %d: esperado um inteiro na expressao para o sinal unario %s ",linha,lista.operadores[i].lexema);
                exit(1);
            }
            pushTipo(pilhaT,"inteiro");
        }else if (precedenciaOperador == 2){
            tipo = popTipo(pilhaT);
            if(strcmp(tipo,"inteiro") == 0 ){
                printf("ERRO Linha %d: esperado um booleano na expressao para o sinal unario %s ",linha,lista.operadores[i].lexema);
                exit(1);
            }
            pushTipo(pilhaT,"booleano");

        }

    }
    free(lista.operadores);
    return popTipo(pilhaT);
}

void analisa_atribuicao(char* ident_proc) {
    ListaOperadores lista;
    char* tipoExpressao;
    int numVariavel = 0,indexUltimaVariavel = 0;

    lexico();
    lista = analisa_expressao();
    tipoExpressao = analisa_tipo_expressao(lista);
    //printf("\nO tipo final eh %s\n",tipoExpressao );

    int index = encontra_func(ident_proc,&numVariavel,&indexUltimaVariavel);

    if(index > 0 && strcmp(tSimb.simbolos[index].lexema,ident_proc) == 0){
        //Atribuição de retorno
        if(strcmp(tipoExpressao,"inteiro") == 0){
            strcpy(tipoExpressao,"funcao_inteiro");
        }else{
            strcpy(tipoExpressao,"funcao_booleano");
        }
        if(strcmp(tSimb.simbolos[index].tipo, tipoExpressao) != 0){
            printf("ERRO Linha %d: Expressao do tipo %s e funcao do tipo %s",linha,tipoExpressao,tSimb.simbolos[index].tipo);
            exit(1);
        }
        gera(-1,"STR",0,-1);
        gera(-1,"DALLOC",indexUltimaVariavel,numVariavel);
        gera(-1,"RETURNF",-1,-1);

    }else{

        index = pesquisa_declvar_tabela(ident_proc);
        if(index == -1){
            printf("ERRO Linha %d: Variavel %s nao declarada",linha, ident_proc);
            exit(1);
        }

        if(strcmp(tSimb.simbolos[index].tipo, tipoExpressao) != 0){
            printf("ERRO Linha %d: Expressao do tipo %s e variavel do tipo %s",linha,tipoExpressao,tSimb.simbolos[index].tipo);
            exit(1);
        }

        gera(-1,"STR",tSimb.simbolos[index].endereco,-1);

    }
}

void analisa_tipo(int qtdVar) {
    if(strcmp(token_atual.simbolo, "sinteiro") != 0 && strcmp(token_atual.simbolo, "sbooleano") != 0) {
        printf("ERRO Linha %d: tipo de variavel invalida\n",linha);
        exit(1);
    }
    else {
        coloca_tipo_tabela(token_atual.lexema, qtdVar);
        lexico();
    }
}

void analisa_leia() {

    lexico();
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico();
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {

            int index = pesquisa_declvar_tabela(token_atual.lexema);
            if(index != -1 && strcmp(tSimb.simbolos[index].tipo, "inteiro") == 0) {
                    gera(-1, "RD", -1, -1);
                    gera(-1, "STR", tSimb.simbolos[index].endereco, -1);
                    lexico();
                    if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                        lexico();
                    }
                    else {
                        printf("ERRO Linha %d: esperado ')'\n",linha);
                        exit(1);
                    }
            }
            else if(index != -1){
                printf("lexema atual: %s", token_atual.lexema);
                printf("Erro Linha %d: leia nao aceita variavel do tipo %s\n",linha, tSimb.simbolos[index].tipo);
                exit(1);
            }
            else {
                printf("ERRO Linha %d: variavel %s nao foi declarada\n",linha, token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO Linha %d:: nome de variavel invalido\n",linha);
            exit(1);
        }
    }
    else {
        printf("ERRO Linha %d: esperado '('\n",linha);
        exit(1);
    }
}

void analisa_escreva() {

    lexico();
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico();
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            int index = pesquisa_declvar_tabela(token_atual.lexema);
            if(index != -1 && strcmp(tSimb.simbolos[index].tipo, "inteiro") == 0) {
                gera(-1, "LDV", tSimb.simbolos[index].endereco, -1);
                gera(-1, "PRN", -1, -1);
                lexico();
                if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                    lexico();
                }
                else {
                    printf("ERRO Linha %d: esperado ')'\n",linha);
                    exit(1);
                }
            }
            else if(index != -1){
                printf("Erro Linha %d: escreva nao aceita variavel do tipo %s\n",linha, tSimb.simbolos[index].tipo);
                exit(1);
            }
            else {
                printf("ERRO Linha %d: variavel %s nao foi declarada\n",linha, token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO Linha %d:: nome de variavel invalido\n",linha);
            exit(1);
        }
    }
    else {
        printf("ERRO Linha %d: esperado '('\n",linha);
        exit(1);
    }
}

void analisa_variaveis() {
    int qtdVar = 0;
    while(strcmp(token_atual.simbolo, "sdoispontos") != 0) {
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            qtdVar++;
            if(!pesquisa_duplicvar_tabela(token_atual.lexema)){ //1 eh verdadeiro
                insere_tabela(token_atual.lexema, "variavel", "", 0);
                lexico();
                if(strcmp(token_atual.simbolo, "svirgula") == 0 || strcmp(token_atual.simbolo, "sdoispontos") == 0) {
                    if(strcmp(token_atual.simbolo, "svirgula") == 0) {
                        lexico();
                        if(strcmp(token_atual.simbolo, "sidentificador") != 0) {
                            printf("ERRO Linha %d: nome de variavel invalido\n", linha);
                            exit(1);
                        }
                    }
                }
                else {
                    printf("ERRO Linha: esperado ',' ou ':'\n");
                    exit(1);
                }
            }
            else {
                printf("ERRO Linha %d: variavel com o nome %s ja existe", linha,token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO Linha %d: nome da variavel invalido\n",linha);
            exit(1);
        }
    }
    lexico();
    analisa_tipo(qtdVar);
    gera(-1, "ALLOC", endereco_mvd, qtdVar); //verificar depois
    endereco_mvd = endereco_mvd + qtdVar;   //verificar depois
}

void analisa_et_variaveis() {
    if(strcmp(token_atual.simbolo, "svar") == 0) {
        lexico();
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            while(strcmp(token_atual.simbolo, "sidentificador") == 0) {
                analisa_variaveis();
                if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                    lexico();
                }
                else {
                    printf("ERRO Linha %d: esta faltando ';'\n",linha);
                    exit(1);
                }
            }
        }
        else {
            printf("ERRO Linha: %d: nome da variavel invalido \n",linha);
            exit(1);
        }
    }
}

void analisa_atrib_chprocedimento() {
    char ident_proc[50];
    strcpy(ident_proc, token_atual.lexema);
    lexico();
    if(strcmp(token_atual.simbolo, "satribuicao") == 0) {
        analisa_atribuicao(ident_proc);
    }
    else {
        analisa_chamada_procedimento(ident_proc);
    }
}

void analisa_se() {
    int auxrot = rotulo;
    int auxrot2;


    lexico();
    ListaOperadores lista;
    char* tipoExpressao;
    lista = analisa_expressao();
    tipoExpressao = analisa_tipo_expressao(lista);
    //printf("\nO tipo final eh %s\n",tipoExpressao );

    if(strcmp("booleano", tipoExpressao) != 0){
        printf("ERRO Linha %d: Expressao tipo inteiro dentro de 'se',esperado booleano",linha);
        exit(1);
    }
    gera(-1, "JMPF",rotulo,-1);
    rotulo++;
    if(strcmp(token_atual.simbolo, "sentao") == 0) {
        lexico();
        analisa_comando_simples();
        gera(-1,"JMP",rotulo,-1);
        gera(auxrot,"NULL",-1,-1);
        auxrot2 = rotulo;
        rotulo++;
        if(strcmp(token_atual.simbolo, "ssenao") == 0) {
            lexico();
            analisa_comando_simples();
        }
    }
    else {
        printf("ERRO Linha %d: esperado entao 'entao' apos comando 'se' \n",linha);
        exit(1);
    }
    gera(auxrot2,"NULL",-1,-1);

}

void analisa_enquanto() {
    int auxrot1, auxrot2;

    auxrot1 = rotulo;
    gera(rotulo, "NULL", -1, -1); //acho que null � uma string aqui
    rotulo++;

    lexico();
    ListaOperadores lista;
    char* tipoExpressao;
    lista = analisa_expressao();
    tipoExpressao = analisa_tipo_expressao(lista);
    //printf("\nO tipo final eh %s\n",tipoExpressao );
    if(strcmp("booleano", tipoExpressao) != 0){
        printf("ERRO Linha %d: Expressao retorna tipo inteiro, esperado tipo booleano",linha);
    }


    if(strcmp(token_atual.simbolo, "sfaca") == 0) {
        auxrot2 = rotulo;
        gera(-1, "JMPF", rotulo, -1);
        rotulo++;
        lexico();
        analisa_comando_simples();
        gera(-1, "JMP", auxrot1, -1);
        gera(auxrot2, "NULL", -1, -1);
    }
    else {
        printf("ERRO Linha %d: esperado faca apos enquanto\n",linha);
        exit(1);
    }
}

void analisa_declaracao_procedimento() {
    lexico();
    char nivel[2];
    strcpy(nivel, "L");
    char nomeProc[50];
    strcpy(nomeProc,token_atual.lexema);
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        if(pesquisa_declproc_tabela()) {
            insere_tabela(token_atual.lexema, "procedimento", nivel, rotulo);
            gera(rotulo, "NULL", -1, -1);
            rotulo++;
            lexico();
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco();
            }
            else {
                printf("ERRO Linha %d: esta faltando ';'\n",linha);
                exit(1);
            }
        }
        else {
            printf("ERRO Linha %d: procedimento %s ja foi declarado\n",linha,token_atual.lexema);
            exit(1);
        }
    }
    else {
        printf("ERRO Linha %d: esta faltando um nome valido para o procedimento\n",linha);
        exit(1);
    }
    remove_tabela(nomeProc);
    gera(-1, "RETURN", -1, -1); //acho que � aqui
}

void analisa_declaracao_funcao() {
    lexico();
    char nivel[2];
    strcpy(nivel, "L");
    char nomeProc[50];
    strcpy(nomeProc,token_atual.lexema);
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        if(pesquisa_declfunc_tabela(token_atual.lexema)) {
            insere_tabela(token_atual.lexema, "-", nivel, rotulo);
            gera(rotulo,"NULL",-1,-1);
            rotulo++;
            lexico();
            if(strcmp(token_atual.simbolo, "sdoispontos") == 0) {
                lexico();
                if(strcmp(token_atual.simbolo, "sinteiro") == 0 || strcmp(token_atual.simbolo, "sbooleano") == 0) {
                    if(strcmp(token_atual.simbolo, "sinteiro") == 0) {
                        strcpy(tSimb.simbolos[tSimb.tamanho -1].tipo, "funcao_inteiro");
                    }
                    else {
                        strcpy(tSimb.simbolos[tSimb.tamanho -1].tipo, "funcao_booleano");
                    }
                    lexico();
                    if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                        analisa_bloco();
                    }
                }
                else {
                    printf("ERRO Linha %d: o tipo de funcao esta invalido\n",linha);
                    exit(1);
                }
            }
            else {
                printf("ERRO Linha %d: esta faltando ':'\n",linha);
                exit(1);
            }
        }
    }
    else {
        printf("ERRO Linha %d: esta faltando o nome da funcao em sua declaracao\n",linha);
        exit(1);
    }
    remove_tabela(nomeProc);
    gera(-1, "RETURNF", -1, -1); //acho que � aqui RETURNF????
}

void analisa_subrotinas() {
    int auxrot;
    int flag = 0;

    if(strcmp(token_atual.simbolo, "sprocedimento") == 0 || strcmp(token_atual.simbolo, "sfuncao") == 0) {
        auxrot = rotulo;
        gera(-1, "JMP", rotulo, -1);
        rotulo++;
        flag = 1;
    }

    while(strcmp(token_atual.simbolo, "sprocedimento") == 0 || strcmp(token_atual.simbolo, "sfuncao") == 0) {
        if(strcmp(token_atual.simbolo, "sprocedimento") == 0) {
            analisa_declaracao_procedimento();
        }
        else {
            analisa_declaracao_funcao();
        }

        if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
            lexico();
        }
        else {
            printf("ERRO %d: esta faltando ';'\n",linha);
            exit(1);
        }
    }

    if(flag == 1) {
        gera(auxrot, "NULL", -1, -1);
    }
}

void analisa_bloco() {
    lexico();
    analisa_et_variaveis();
    analisa_subrotinas();
    analisa_comandos();
}

void analisa_fator(ListaOperadores *lista, Pilha* pilhapos) {
    int ind;
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        //printf("%s\n", token_atual.lexema);
        insere_lista(token_atual,lista);
        if(pesquisa_tabela(token_atual.lexema, &ind)) {
                if((strcmp(tSimb.simbolos[ind].tipo, "funcao_inteiro") == 0) || (strcmp(tSimb.simbolos[ind].tipo, "funcao_booleano") == 0)) {
                    analisa_chamada_funcao();
                }
                lexico();

        }
        else {
            printf("Erro linha %d: %s nao e nome de funcao ou variavel\n",linha,token_atual.lexema);
            exit(1);
        }
    }
    else if(strcmp(token_atual.simbolo, "snumero") == 0) {
        insere_lista(token_atual,lista);
        lexico();
    }
    else if(strcmp(token_atual.simbolo, "snao") == 0) {
        //unario
        trataOperadorPos(lista,pilhapos);
        lexico();
        analisa_fator(lista,pilhapos);
    }
    else if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico();
        ListaOperadores expressaoParenteses = analisa_expressao();
        if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
            for(int i = 0; i<expressaoParenteses.tamanho;i++){
                insere_lista(expressaoParenteses.operadores[i], lista);
            }
            lexico();
        }
        else {
            printf("\n%s\n",token_atual.lexema);
            printf("\n%s\n",token_atual.simbolo);
            printf("ERRO Linha %d: esta faltando ')' \n",linha);
            exit(1);
        }
    }
    else if(strcmp(token_atual.simbolo, "sverdadeiro") == 0 || strcmp(token_atual.simbolo, "sfalso") == 0) {
        insere_lista(token_atual,lista);
        lexico();
    }
    else {
        printf("%s", token_atual.simbolo);
        printf("ERRO Linha %d: %s nao e valido na expressao\n",linha,token_atual.lexema);
        exit(1);
    }
}

void analisa_termo(ListaOperadores *lista,Pilha* pilhapos) {
    analisa_fator( lista,pilhapos);
    while(strcmp(token_atual.simbolo, "smult") == 0 || strcmp(token_atual.simbolo, "sdiv") == 0 || strcmp(token_atual.simbolo, "se") == 0) {
        trataOperadorPos(lista,pilhapos);
        lexico();
        analisa_fator(lista,pilhapos);
    }
}

void analisa_expressao_simples(ListaOperadores* lista, Pilha* pilhapos) {
    if(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0) {
        //operadores unarios

        if(strcmp(token_atual.simbolo, "smenos") == 0){
            strcpy(token_atual.simbolo,"#");
        }else{
             strcpy(token_atual.simbolo,"@");
        }
        trataOperadorPos(lista,pilhapos);
        lexico();
    }
    analisa_termo(lista,pilhapos);
    while(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0 || strcmp(token_atual.simbolo, "sou") == 0) {
        trataOperadorPos(lista,pilhapos);
        lexico();
        analisa_termo(lista,pilhapos);
    }
}

void analisa_comandos() {
    if(strcmp(token_atual.simbolo, "sinicio") == 0) {
        lexico();
        analisa_comando_simples();
        while(strcmp(token_atual.simbolo, "sfim") != 0) {
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                lexico();
                if(strcmp(token_atual.simbolo, "sfim") != 0) {
                    analisa_comando_simples();
                }
            }
            else {

                printf("ERRO Linha %d: esta faltando ;\n",linha);
                exit(1);
            }

        }
        lexico();
    }
    else {
        printf("ERRO Linha %d: esta faltando inicio apos comando\n",linha);
        exit(1);
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

    arquivo = fopen("teste_mvd.txt", "r");

    if(arquivo == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return -1;
    }

    caractere = fgetc(arquivo);

    int teste = 0;
    lexico();

    rotulo = 1;

    if(strcmp(token_atual.simbolo, "sprograma") == 0){
        lexico();
        if(strcmp(token_atual.simbolo, "sidentificador") == 0){
            gera(-1, "START", -1, -1);
            gera(-1,"ALLOC",0,1);
            insere_tabela(token_atual.lexema, "nomedeprograma", "L", 0);
            lexico(&fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco();
                if(strcmp(token_atual.simbolo, "sponto") == 0) {
                    if(lexico() == 2) {
                        int k = 1;
                        int varContador = 0;
                        int index = 0;
                        if(strcmp(tSimb.simbolos[k].escopo,"L") != 0){
                            index = tSimb.simbolos[k].endereco;
                            while(strcmp(tSimb.simbolos[k].escopo,"L") != 0 && k != tSimb.tamanho-1){
                                  varContador++;
                                  k++;

                            }
                            gera(-1,"DALLOC",index,varContador);
                        }
                        gera(-1, "DALLOC", 0, 1);
                        gera(-1, "HLT", -1, -1);
                        printf(" Sucesso!");
                    }
                    else {
                        printf("ERRO");
                        exit(1);
                    }
                }
                else {
                    printf("ERRO Linha %d programa: esperado <.>\n",linha);
                    exit(1);
                }
            }
            else {
                printf("ERRO Linha %d: esta faltando ;",linha);
                exit(1);
            }
            //erro duplicvar
        }
        else{
            printf("ERRO Linha %d: esta faltando nome do programa\n",linha);
            exit(1);
        }
    }
    else {
        printf("ERRO Linha %d: esta faltando programa\n",linha);
        exit(1);
    }

    while(teste != 2){
        teste = lexico();
    }

    if(arquivo_obj != NULL) {
        fclose(arquivo_obj);
        arquivo_obj = NULL;
    }

    fclose(arquivo);
    free(tSimb.simbolos);
    return 0;
}
