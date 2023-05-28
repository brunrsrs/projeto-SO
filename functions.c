#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "functions.h"

int memoriaOcupada=0;

void menu() {
    printf("\n[1] Adicionar Thread\n[2] Listar Threads\n[3] Estado memória\n[0] Sair\nComando: ");
}

//inicializa o banco de dados
int inicializaBCP(bcp *b) {
    if (!b) {
        printf("\n\tNão foi possivel inicializar o BCP\n");
        return 0;
    }

    b->prog = NULL;
    b->prox = NULL;
    b->tamTotal = 0;
    return 1;
}

    // Simular a operação de carregamento do processo na memória
void memLoadReq(programa *processo, bcp *b) {
    printf("\nCarregando processo na memória...\n");
    
    b->tamTotal+=processo->tamanho;   //memoria ocupada pode ser uma var global sem muitos problemas acho

    if(b->tamTotal>1000000000) {
        printf("\nMemória cheia, não é possível ocupar mais dela.\n");
        b->tamTotal-=processo->tamanho;
    }

    else memLoadFinish();
}

void memLoadFinish(){//essa daqui so sinaliza que terminou de carregar????
    printf("\nProcesso carregado na memória.\n");

    //em algum lugar dessas funções de carregar/encerrar vai ter que chamar o semáforo provavelmente, pra fechar de fato os "processos"
}

// Função para simular a criação de um processo no BCP
void processCreate(programa *processo, bcp *b) {
    //a ideia é pegar os valores do header e criar o processo aqui, já atribuindo o thread dele e tudo mais
    //muito provavelmente vai ter que ter uma lista de objetos ou parecido pra adicionar mais um processo sempre que necessario
    memLoadReq(processo, b);
}

void processFinish(programa processo, bcp b){
    printf("\nEncerrando processo...\n");
    memoriaOcupada-=processo.tamanho;
    printf("\nProcesso encerrado.\n");
}

int programRead(struct programa *pg, char nomeProcesso[10], struct bcp *b){ //synP de synthethic program
    //vai ser necessária a leitura dos "programas" (arquivos), essa função é pra isso
    //acho que vai acabar tendo que criar um objeto por programa ou fazer um vetor deles (e fazer funcionar)
    FILE *synP;
    char comando[5];
    int valorComando;

    synP = fopen(nomeProcesso, "r");    //nao consegui fazer abrir de acordo com o que o usuario escreve
    if (synP == NULL) {                                 //só colocar o nome do arquivo bobinho
        printf("\nErro ao abrir o programa\n"); 
        return 0;
    }

    //pega os valores do header do txt
    char auxTxt[10];
    char auxChar;
    fscanf(synP, "%s", auxTxt);
    strcpy(pg->nome, auxTxt);

    fscanf(synP, "%d\n", &pg->ident);
    fscanf(synP, "%d\n", &pg->prioridade);
    fscanf(synP, "%d\n", &pg->tamanho);

    do {
        auxChar = fgetc(synP);
        strncat(pg->semaforos, &auxChar, 1);
    } while (auxChar!='\n');

//    processCreate(pg, b);

    int read=0, exec=0, write=0, print=0, ps=0, vs=0;

    //pega os comandos do txt
    while(!feof(synP)) {
        fscanf(synP, "%s", comando);
        printf("\nComando: %s\n", comando);//pra ver melhor
        if(strcmp("exec", comando) == 0){//a ideia é verificar se ele encaixa em alguma das funções, pra depois ler o int e fazer a operação
            fscanf(synP, " %d\n", &valorComando);
//            func_exec(valorComando);//o valor aqui é o tempo de execução
        }
        else if(strcmp("read", comando) == 0){
            fscanf(synP, " %d\n", &valorComando);
//            func_read(valorComando);//o valor aqui é a trilha a ser lida
        }
        else if(strcmp("write", comando) == 0){
            fscanf(synP, " %d\n", &valorComando);
//            func_write(valorComando);//o valor aqui é a trilha a ser escrita
        }
        else if(strcmp("print", comando) == 0){
            fscanf(synP, " %d\n", &valorComando);
//            func_print(valorComando);//o valor aqui o tempo que o programa imprimirá resultados(?)
        }
        else if(strcmp("P(s)", comando) == 0){//acessa região crítica guardada por s
            getc(synP);
//            semaphoreP(s);//tem que ver exatamente do que se trata "s" e "t"
        }
        else if(strcmp("V(s)", comando) == 0){//libera região crítica guardada por s
            getc(synP);
//            semaphoreV(s);
        }
        else if(strcmp("V(t)", comando) == 0){
            getc(synP);
//            semaphoreV(t);
        }
        else if(strcmp("P(t)", comando) == 0){
            getc(synP);
//            semaphoreP(t); 
        }
        else printf("\nO comando %s não é válido", comando);
    }

    //so pra ver os valores
    printf("\nNome: %s\n", pg->nome);
    printf("Ident: %d\n", pg->ident);
    printf("Prioridade: %d\n", pg->prioridade);
    printf("Tamanho: %d\n", pg->tamanho);
    printf("Semaforos: %s\n", pg->semaforos);

    printf("qtd exec: %d\n", exec);
    printf("qtd read: %d\n", read);
    printf("qtd write: %d\n", write);
    printf("qtd print: %d\n", print);
    printf("qtd ps: %d\n", ps);
    printf("qtd vs: %d\n", vs);

    fclose(synP);
    return 1;
}


//Algumas funções do escalonamento

int interruptControl() {

}

int sysCall() {
    //processInterrupt()
}