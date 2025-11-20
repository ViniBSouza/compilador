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

void analisa_comandos(fila_tokens *fila);
void analisa_comando_simples(fila_tokens *fila);
void analisa_expressao_simples(fila_tokens *fila, ListaOperadores *lista,Pilha *pilhapos);
void analisa_termo(fila_tokens *fila, ListaOperadores *lista,Pilha *pilhapos);
void analisa_fator(fila_tokens *fila, ListaOperadores *lista,Pilha *pilhapos);
void analisa_declaracao_procedimento(fila_tokens *fila);
void analisa_subrotinas(fila_tokens *fila);
void analisa_bloco(fila_tokens *fila);

void gera(int rot, const char *arg1, int arg2, int arg3) {
    if(arquivo_obj == NULL) {
        printf("Erro: arquivo .obj nao foi aberto\n");
        return;
    }

    if(rot >= 0) {
        fprintf(arquivo_obj, "%d   ", rot);
    }
    else {
        fprintf(arquivo_obj, "    ");
    }

    fprintf(arquivo_obj, "%s       ", arg1);

    if(arg2 >= 0) {
        fprintf(arquivo_obj, "%d   ", arg2);
    }
    else {
        fprintf(arquivo_obj, "    ");
    }

    if(arg3 >= 0) {
        fprintf(arquivo_obj, "%d   \n", arg3);
    }
    else {
        fprintf(arquivo_obj, "    \n");
    }
}

void insere_tabela(const char *lexema, const char *tipo, const char *escopo, int rot) {
    tSimb.tamanho++;

    simbolo *temp = realloc(tSimb.simbolos, tSimb.tamanho * sizeof(simbolo));
    if(temp == NULL){
        printf("Erro na aloca��o de mem�ria\n");
        free(tSimb.simbolos);
        exit(1);
    }

    tSimb.simbolos = temp;

    simbolo *novo = &tSimb.simbolos[tSimb.tamanho - 1];
    strcpy(novo->lexema, token_atual.lexema);
    strcpy(novo->tipo, tipo);
    strcpy(novo->escopo, escopo);
    novo->endereco = endereco_tabela;
    endereco_tabela++;
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
    printf("\n--- Tabela de S�mbolos ---\n");
    for (int i = 0; i < tSimb.tamanho; i++) {
        printf("Lexema: %-10s | Tipo: %-10s | Escopo: %s | Endereco: %d\n",
               tSimb.simbolos[i].lexema,
               tSimb.simbolos[i].tipo,
               tSimb.simbolos[i].escopo,
               tSimb.simbolos[i].endereco);
    }
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
            printf("ERRO: variavel ou funcao nao declarada");
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
            printf("ERRO: variavel nao declarada");
            exit(1);
        }
    }

    return -1;
}

int pesquisa_declproc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro: nao e possivel criar um procedimento como o nome %s pois ja existe uma variavel/funcao/procedimento\n", token_atual.lexema);
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

int pesquisa_declfunc_tabela() {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(token_atual.lexema, tSimb.simbolos[i].lexema) == 0) {
            printf("Erro: ja existe uma variavel/funcao/procedimento com o nome %s\n", token_atual.lexema);
            exit(1);
            //printf("%s", tSimb.simbolos[i].lexema);
            return 0;
        }
    }

    return 1;
}

//fazer
void analisa_chamada_funcao(fila_tokens *fila) {
    /* pesquisa_declvarfunc_tabela(token_atual.lexema);
    printf("oi sou o %s\n", token_atual.lexema);
    lexico(fila);
    */
}

//fazer
int analisa_chamada_procedimento(fila_tokens *fila, char *ident_proc) {
    for(int i = tSimb.tamanho - 1; i >= 0; i--) {
        if(strcmp(ident_proc, tSimb.simbolos[i].lexema) == 0) {
            if(strcmp(tSimb.simbolos[i].tipo, "procedimento") == 0) {
                //printf("%s", tSimb.simbolos[i].lexema);
                return 0; //verdadeiro: proc existe
            }
            else {
                return 1;
            }
        }
    }
    printf("Erro: procedimento %s nao existe", ident_proc);
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

ListaOperadores analisa_expressao(fila_tokens *fila) {
    Pilha * pilhapos = criaPilha();
    ListaOperadores lista;
    lista.operadores = NULL;
    lista.tamanho = 0;
    analisa_expressao_simples(fila,&lista,pilhapos);
    if(strcmp(token_atual.simbolo, "smaior") == 0 || strcmp(token_atual.simbolo, "smaiorig") == 0 ||
       strcmp(token_atual.simbolo, "sig") == 0 || strcmp(token_atual.simbolo, "smenor") == 0 ||
       strcmp(token_atual.simbolo, "smenorig") == 0 || strcmp(token_atual.simbolo, "sdif") == 0) {
        trataOperadorPos(&lista,pilhapos);
        lexico(fila);
        analisa_expressao_simples(fila,&lista,pilhapos);
    }

    while(pilhapos->topo != NULL){
        insere_lista(pop(pilhapos),&lista);
    }

    printf("OLHA SO A MINHA EXPRESSAO POSFIXE EBAAAA: \n");
    for(int i = 0; i< lista.tamanho;i++){
        printf("%s",lista.operadores[i].lexema);
    }

    free(pilhapos);
    return lista;
}

void verifica_tipo_variavel_funcao(PilhaTipo *pilhaT,char* lexema){
    int index;
    pesquisa_tabela(lexema,&index);
    if(index == 0){
        printf("ERRO: variavel/funcao nao declarada");
        exit(1);
    }

    if(strcmp(tSimb.simbolos[index].tipo,"nomedeprograma") == 0 || strcmp(tSimb.simbolos[index].tipo,"procedimento") == 0){
        printf("ERRO: %s nao eh variavel/funcao", lexema);
        exit(1);
    }
    if(strcmp(tSimb.simbolos[index].tipo, "funcao_inteiro") == 0) {
        pushTipo(pilhaT,"inteiro");
        gera(-1,"CALL",tSimb.simbolos[index].endereco,-1);
        gera(-1,"LDV",0,-1);
    }else if(strcmp(tSimb.simbolos[index].tipo,"inteiro") == 0){
        pushTipo(pilhaT,"inteiro");
        gera(-1,"LDV",tSimb.simbolos[index].endereco,-1);
    }else if(strcmp(tSimb.simbolos[index].tipo,"funcao_booleano") == 0){
        pushTipo(pilhaT,"booleano");
        gera(-1,"CALL",tSimb.simbolos[index].endereco,-1);
        gera(-1,"LDV",0,-1);
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
        }else if(strcasecmp(lista.operadores[i].simbolo,"snumero") == 0){
            pushTipo(pilhaT,"inteiro");
            gera(-1,"LDC",atoi(lista.operadores[i].lexema),-1);
        }else if(precedenciaOperador >= 4 && precedenciaOperador < 7 ){
            for(j = 0; j< 2; j++){
                tipo = popTipo(pilhaT);
                if(strcmp(tipo,"inteiro") != 0){
                    printf("ERRO: esperado dois inteiros na expressao para o sinal %s ",lista.operadores[i].lexema);
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
                    printf("ERRO: esperado dois booleanos na expressao para o sinal %s ",lista.operadores[i].lexema);
                    exit(1);
                }
            }
            pushTipo(pilhaT,"booleano");
        }else if(precedenciaOperador == 7){
            tipo = popTipo(pilhaT);
            if(strcmp(tipo,"intero") != 0 ){
                printf("ERRO: esperado um inteiro na expressao para o sinal unario %s ",lista.operadores[i].lexema);
                exit(1);
            }
            pushTipo(pilhaT,"inteiro");
        }else if (precedenciaOperador == 2){
            tipo = popTipo(pilhaT);
            if(strcmp(tipo,"inteiro") == 0 ){
                printf("ERRO: esperado um booleano na expressao para o sinal unario %s ",lista.operadores[i].lexema);
                exit(1);
            }
            pushTipo(pilhaT,"booleano");

        }

    }

    return popTipo(pilhaT);
}

void analisa_atribuicao(fila_tokens *fila ,char* ident_proc) {
    ListaOperadores lista;
    char* tipoExpressao;
    int index = pesquisa_declvar_tabela(ident_proc);
    if(index == -1){
        printf("ERRO: Variavel %s nao declarada", ident_proc);
        exit(1);
    }


    lexico(fila);
    lista = analisa_expressao(fila);
    tipoExpressao = analisa_tipo_expressao(lista);
    printf("\nO tipo final eh %s\n",tipoExpressao );
    if(strcmp(tSimb.simbolos[index].tipo, tipoExpressao) != 0){
        printf("ERRO: Expressao do tipo %s e variavel do tipo %s",tipoExpressao,tSimb.simbolos[index].tipo);
        exit(1);
    }

}

void analisa_tipo(fila_tokens *fila, int qtdVar) {
    if(strcmp(token_atual.simbolo, "sinteiro") != 0 && strcmp(token_atual.simbolo, "sbooleano") != 0) {
        printf("ERRO analisa_tipo: tipo de variavel invalida\n");
        exit(1);
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
                    gera(-1, "RD", -1, -1);
                    gera(-1, "STR", tSimb.simbolos[index].endereco, -1);
                    lexico(fila);
                    if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                        lexico(fila);
                    }
                    else {
                        printf("ERRO analisa_leia: esperado <)>\n");
                        exit(1);
                    }
            }
            else if(index != -1){
                printf("lexema atual: %s", token_atual.lexema);
                printf("Erro: leia nao aceita variavel do tipo %s\n", tSimb.simbolos[index].tipo);
                exit(1);
            }
            else {
                printf("ERRO variavel %s nao foi declarada\n", token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO analisa_leia: esperado identificador\n");
            exit(1);
        }
    }
    else {
        printf("ERRO analisa_leia: esperado <(>\n");
        exit(1);
    }
}

void analisa_escreva(fila_tokens *fila) {
    int cont = 0;
    lexico(fila);
    if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            cont++;
            int index = pesquisa_declvar_tabela(token_atual.lexema);
            if(index != -1 && strcmp(tSimb.simbolos[index].tipo, "inteiro") == 0) {
                gera(-1, "LDV", cont, -1); // � cont mesmo?
                gera(-1, "PRN", -1, -1);
                lexico(fila);
                if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
                    lexico(fila);
                }
                else {
                    printf("ERRO analisa_escreva: esperado <)>\n");
                    exit(1);
                }
            }
            else if(index != -1){
                printf("Erro: escreva nao aceita variavel do tipo %s\n", tSimb.simbolos[index].tipo);
                exit(1);
            }
            else {
                printf("ERRO: variavel %s nao foi declarada\n", token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO analisa_escreva: esperado identificador\n");
            exit(1);
        }
    }
    else {
        printf("ERRO analisa_escreva: esperado <(>\n");
        exit(1);
    }
}

void analisa_variaveis(fila_tokens *fila) {
    int qtdVar = 0;
    while(strcmp(token_atual.simbolo, "sdoispontos") != 0) {
        if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
            qtdVar++;
            if(!pesquisa_duplicvar_tabela(token_atual.lexema)){ //1 eh verdadeiro
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
                    exit(1);
                }
            }
            else {
                printf("ERRO: variavel com o nome %s ja existe", token_atual.lexema);
                exit(1);
            }
        }
        else {
            printf("ERRO analisa_variaveis: esperado identificador\n");
            exit(1);
        }
    }
    lexico(fila);
    analisa_tipo(fila, qtdVar);
    gera(-1, "ALLOC", endereco_mvd, qtdVar); //verificar depois
    endereco_mvd = endereco_mvd + qtdVar;   //verificar depois
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
                    exit(1);
                }
            }
        }
        else {
            printf("ERRO analisa_et_variaveis: esperado identificador\n");
            exit(1);
        }
    }
}

void analisa_atrib_chprocedimento(fila_tokens *fila) {
    char ident_proc[50];
    strcpy(ident_proc, token_atual.lexema);
    lexico(fila);
    if(strcmp(token_atual.simbolo, "satribuicao") == 0) {
        analisa_atribuicao(fila,ident_proc);
    }
    else {
        analisa_chamada_procedimento(fila, ident_proc);
    }
}

void analisa_se(fila_tokens *fila) {
    lexico(fila);
    ListaOperadores lista;
    char* tipoExpressao;
    lista = analisa_expressao(fila);
    tipoExpressao = analisa_tipo_expressao(lista);
    printf("\nO tipo final eh %s\n",tipoExpressao );
    if(strcmp("booleano", tipoExpressao) != 0){
        printf("ERRO: Expressao tipo inteiro dentro de enquanto");
    }
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
        exit(1);
    }
}

void analisa_enquanto(fila_tokens *fila) {
    int auxrot1, auxrot2;

    auxrot1 = rotulo;
    gera(rotulo, "NULL", -1, -1); //acho que null � uma string aqui
    rotulo++;

    lexico(fila);
    ListaOperadores lista;
    char* tipoExpressao;
    lista = analisa_expressao(fila);
    tipoExpressao = analisa_tipo_expressao(lista);
    printf("\nO tipo final eh %s\n",tipoExpressao );
    if(strcmp("booleano", tipoExpressao) != 0){
        printf("ERRO: Expressao tipo inteiro dentro de enquanto");
    }


    if(strcmp(token_atual.simbolo, "sfaca") == 0) {
        auxrot2 = rotulo;
        gera(-1, "JMPF", rotulo, -1);
        rotulo++;
        lexico(fila);
        analisa_comando_simples(fila);
        gera(-1, "JMP", auxrot1, -1);
        gera(auxrot2, NULL, -1, -1);
    }
    else {
        printf("ERRO analisa_enquanto: esperado <faca>\n");
        exit(1);
    }
}

void analisa_declaracao_procedimento(fila_tokens *fila) {
    lexico(fila);
    char nivel[2];
    strcpy(nivel, "L");
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        if(pesquisa_declproc_tabela()) {
            insere_tabela(token_atual.lexema, "procedimento", nivel, rotulo);
            gera(rotulo, "NULL", -1, -1);
            rotulo++;
            lexico(fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(fila);
            }
            else {
                printf("ERRO analisa_declaracao_procedimento: esperado <;>\n");
                exit(1);
            }
        }
        else {
            printf("ERRO: Procedimento ja foi declarado\n");
            exit(1);
        }
    }
    else {
        printf("ERRO analisa_declaracao_procedimento: esperado identificador\n");
        exit(1);
    }
    remove_tabela();
    gera(-1, "RETURN", -1, -1); //acho que � aqui
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
                    exit(1);
                }
            }
            else {
                printf("ERRO analisa_declaracao_funcao: esperado <:>\n");
                exit(1);
            }
        }
    }
    else {
        printf("ERRO analisa_declaracao_funcao: esperado identificador\n");
        exit(1);
    }
    remove_tabela();
    gera(-1, "RETURN", -1, -1); //acho que � aqui RETURNF????
}

void analisa_subrotinas(fila_tokens *fila) {
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
            exit(1);
        }
    }

    if(flag == 1) {
        gera(auxrot, "NULL", -1, -1);
    }
}

void analisa_bloco(fila_tokens *fila) {
    lexico(fila);
    analisa_et_variaveis(fila);
    analisa_subrotinas(fila);
    analisa_comandos(fila);
}

void analisa_fator(fila_tokens *fila,ListaOperadores *lista, Pilha* pilhapos) {
    int ind;
    if(strcmp(token_atual.simbolo, "sidentificador") == 0) {
        //printf("%s\n", token_atual.lexema);
        insere_lista(token_atual,lista);
        if(pesquisa_tabela(token_atual.lexema, &ind)) {
                if((strcmp(tSimb.simbolos[ind].tipo, "funcao_inteiro") == 0) || (strcmp(tSimb.simbolos[ind].tipo, "funcao_booleano") == 0)) {
                    analisa_chamada_funcao(fila);
                }
                else {
                    lexico(fila);
                }
        }
        else {
            printf("Erro: identificador nao encontrado\n");
            exit(1);
        }
    }
    else if(strcmp(token_atual.simbolo, "snumero") == 0) {
        insere_lista(token_atual,lista);
        lexico(fila);
    }
    else if(strcmp(token_atual.simbolo, "snao") == 0) {
        //unario
        trataOperadorPos(lista,pilhapos);
        lexico(fila);
        analisa_fator(fila,lista,pilhapos);
    }
    else if(strcmp(token_atual.simbolo, "sabre_parenteses") == 0) {
        lexico(fila);
        ListaOperadores expressaoParenteses = analisa_expressao(fila);
        if(strcmp(token_atual.simbolo, "sfecha_parenteses") == 0) {
            for(int i = 0; i<expressaoParenteses.tamanho;i++){
                insere_lista(expressaoParenteses.operadores[i], lista);
            }
            lexico(fila);
        }
        else {
            printf("ERRO analisa_fator: esperado <)>\n");
            exit(1);
        }
    }
    else if(strcmp(token_atual.lexema, "verdadeiro") == 0 || strcmp(token_atual.lexema, "falso") == 0) {
        insere_lista(token_atual,lista);
        lexico(fila);
    }
    else {
        printf("%s", token_atual.simbolo);
        printf("ERRO analisa_fator: entrada invalida\n");
        exit(1);
    }
}

void analisa_termo(fila_tokens *fila,ListaOperadores *lista,Pilha* pilhapos) {
    analisa_fator(fila, lista,pilhapos);
    while(strcmp(token_atual.simbolo, "smult") == 0 || strcmp(token_atual.simbolo, "sdiv") == 0 || strcmp(token_atual.simbolo, "se") == 0) {
        trataOperadorPos(lista,pilhapos);
        lexico(fila);
        analisa_fator(fila,lista,pilhapos);
    }
}

void analisa_expressao_simples(fila_tokens *fila,ListaOperadores* lista, Pilha* pilhapos) {
    if(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0) {
        //operadores unarios
        strcpy(token_atual.simbolo,"@");
        if(strcmp(token_atual.simbolo, "smenos") == 0){
            strcpy(token_atual.simbolo,"#");
        }
        trataOperadorPos(lista,pilhapos);
        lexico(fila);
    }
    analisa_termo(fila,lista,pilhapos);
    while(strcmp(token_atual.simbolo, "smais") == 0 || strcmp(token_atual.simbolo, "smenos") == 0 || strcmp(token_atual.simbolo, "sou") == 0) {
        trataOperadorPos(lista,pilhapos);
        lexico(fila);
        analisa_termo(fila,lista,pilhapos);
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
                imprimir_lista_tokens(fila);
                printf("ERRO analisa_comandos: esperado <;>\n");
                exit(1);
            }

        }
        lexico(fila);
    }
    else {
        printf("ERRO analisa_comandos: esperado sinicio\n");
        printf("%s", token_atual.lexema);
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
    lexico(&fila);

    rotulo = 1;

    if(strcmp(token_atual.simbolo, "sprograma") == 0){
        lexico(&fila);
        if(strcmp(token_atual.simbolo, "sidentificador") == 0){
            gera(-1, "START", -1, -1);
            insere_tabela(token_atual.lexema, "nomedeprograma", "L", 0);
            lexico(&fila);
            if(strcmp(token_atual.simbolo, "sponto_virgula") == 0) {
                analisa_bloco(&fila);
                if(strcmp(token_atual.simbolo, "sponto") == 0) {
                    if(lexico(&fila) == 2) {
                        gera(-1, "HLT", -1, -1);
                        printf(" Sucesso!");
                    }
                    else {
                        printf("ERRO");
                        exit(1);
                    }
                }
                else {
                    printf("ERRO programa: esperado <.>\n");
                    exit(1);
                }
            }
            else {
                printf("ERRO programa: esperado <;>");
                exit(1);
            }
            //erro duplicvar
        }
        else{
            printf("ERRO programa: identificador nao encontrado\n");
            exit(1);
        }
    }
    else {
        printf("ERRO programa: sprograma nao encontrado\n");
        exit(1);
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
