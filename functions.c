#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "functions.h"

#define GIGA 1000000000 //o limite é de memória é 1 GB
#define nBlocos 125000000   //cada bloco tem 8 bytes, então cabem 125 milhões de blocos nisso

extern bcp b;
extern programa auxPg;
extern char comando[20];
extern int active;
extern blocos bloco[nBlocos];

//menu com opções
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
    b->tamTotal = 0;
    return 1;
}

//inicializa o struct programa
int inicializaPg(programa *p) {
    if (!p) {
        printf("\n\tNão foi possivel inicializar programa\n");
        return 0;
    }

    p->nome[0] = '0';
    p->ident = 0;
    p->prior = 0;
    p->semaforos[0] = '\0';
    p->prox = NULL;
    p->tamanho=0;
    p->tempo=0;
    return 1;
}

void *exec(void* banco) {
    bcp *b = (bcp*)banco;
    int counter=0;
    char auxChar = '0';
    char auxString[10];
    int auxInt;
    FILE* reader;
    while(1) {
        if (b->prog) {
                strcpy(comando, "Lendo dados");
                active = 1;
                reader = fopen(b->prog->nome, "r");

                auxPg.tempo = b->prog->tempo;
                fscanf(reader, "%s\n", auxString);
                strcpy(auxPg.nome, auxString);
                fscanf(reader, "%d\n", &auxPg.ident);
                fscanf(reader, "%d\n", &auxPg.prior);
                fscanf(reader, "%d\n", &auxPg.tamanho);
                
                auxChar = getc(reader);
                while (auxChar!='\n'){
                    strncat(auxPg.semaforos, &auxChar, 1);
                    auxChar = getc(reader);
                } 

                auxChar = getc(reader);

                while (!feof(reader)) {
                    fscanf(reader, "%s", comando);

                    if(strcmp("exec", comando) == 0){ //ele pediu pra executar por auxInt tempo
                        fscanf(reader, " %d\n", &auxInt);
                        while(auxInt>=0) {
                            usleep(1000);
                            auxInt--;
                            auxPg.tempo--;
                        }
                    }
                    else if(strcmp("read", comando) == 0){
                        fscanf(reader, " %d\n", &auxInt);
                    }
                    else if(strcmp("write", comando) == 0){
                        fscanf(reader, " %d\n", &auxInt);
                    }
                    else if(strcmp("print", comando) == 0){ //ele pediu pra executar por auxInt tempo
                        fscanf(reader, " %d\n", &auxInt);
                        while(auxInt>=0) {
                            usleep(1000);
                            auxInt--;
                            auxPg.tempo--;
                        }
                    }
                    else if(strcmp("P(s)", comando) == 0){
                        getc(reader);
                    }
                    else if(strcmp("V(s)", comando) == 0){
                        getc(reader);
                    }
                    else if(strcmp("V(t)", comando) == 0){
                        getc(reader);
                    }
                    else if(strcmp("P(t)", comando) == 0){
                        getc(reader);
                    }
                    else
                        auxChar = getc(reader);
                
                }
            fclose(reader);
            b->prog = b->prog->prox;
//            processFinish(b->prog->tamanho, b);
        }
    active=0;
    sleep(1);
    }
}

//funções lista
int inserir(programa *p, bcp *b){   //p é o programa a ser inserido, b é a bcp mesmo
    if(b->tamTotal+p->tamanho <= GIGA) {
        b->tamTotal+=p->tamanho;
//        atribuiPagina(b, p);

        if (!p || !b) {
            printf("\n\tNão foi possível inicializar\n");
            return 0;
        }

        //insere vazia
        if(b->prog == NULL) {
            b->prog = p;
            b->prog->prox = NULL;
            return 1;
        }
        

        programa *aux = b->prog;


        //caso precise inserir no começo, inserir na 2° posição para não quebrar o que está rodando
        if (aux->prox && aux->tamanho < aux->prox->tamanho) {
            if (aux->prox == NULL) { //caso não tenha mais nada depois
                aux->prox = p;
                p->prox = NULL;
            }
            aux->prox = b->prog;
            b->prog = aux;
            return 1;
        }

        while (aux->prox && aux->tempo > aux->prox->tempo)
            aux = aux->prox;

        //insere no final
        if (aux->prox == NULL) {
            aux->prox = p;
            p->prox = NULL;
        }

        //insere no meio
        p->prox = aux->prox;
        aux->prox = p;
        return 1;
    }
    else{
        printf("\n\tO limite máximo de armazenamento foi atingido\n");
        return 0;
    }
}

//funções pras páginas
void atribuiPagina(bcp *b, programa *p){ //ocupa o (teto do tamanho ocupado/8) páginas
    p->pagina = (int*) malloc((ceil(b->tamTotal/8)) * sizeof(int));    //vai precisar de um vetor dinamico pra guardar as paginas ocupadas
    
    for(int i=0 ; i < ceil(b->tamTotal/8) ; i++){
        bloco[i].ocupado=1;
        bloco[i].pagina->refBit = 1;
        bloco[i].pagina->numPag = i;
        p->pagina[i] = ceil(b->tamTotal/8) + i;//se um processo ocupa 1 pagina, ele terá a pagina[0] atrelado à pagina 20 da memória, por exemplo
    }
}

void removePagina(bcp *b, int tam){//'remove' paginas a partir da ultima pagina ocupada
    for(int i=ceil((b->tamTotal/8)-1); i > ceil((b->tamTotal/8)-1) - ceil(tam/8) ; i--){
        bloco[i].ocupado = 0;
        free(bloco[i].pagina);
    }
}

int processFinish(int tam, bcp *b){ //para remover um processo quando seu tempo zera

    if (!b || !b->prog) {
        printf("\nAlgo deu errado no processo");
        return 0;
    }

    if(b->tamTotal - tam < 0){   //verificando se a subtração dá um valor inválido
        printf("\nAlgo deu errado na remoção");
        return 0;
    }
    programa *aux = b->prog;

    if(aux->prox) //como a inserção é ordenada, a remoção será sempre no início da lista
        b->prog = aux->prox;
    
    else 
        b->prog = NULL;   //quando nao resta nada depois da remoção
    
    b->tamTotal -= tam;
//    removePagina(b, tam);

//    free(aux);
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

int programRead(struct programa *pg, char nomeProcesso[10], struct bcp *b){ //synP de synthethic program
    FILE *synP;
    char comando[5];
    int valorComando;
    char auxTxt[10];
    char auxChar;
    int aux;

    synP = fopen(nomeProcesso, "r");    //nao consegui fazer abrir de acordo com o que o usuario escreve
    if (synP == NULL) {                                 //só colocar o nome do arquivo bobinho
        printf("\nErro ao abrir o programa\n"); 
        return 0;
    }

    pg->tempo = 0;
    //pega os valores do header do txt
    strcpy(pg->nome, nomeProcesso); //Define o nome do programa como nome do arquivo inicializavel
    
    fscanf(synP, "%s", auxTxt); //Pula o nome do arquivo
    fscanf(synP, "%d\n", &aux); //Pula o identificador
    fscanf(synP, "%d\n", &aux); //Pula a prioridade
    fscanf(synP, "%d\n", &pg->tamanho); //Lê o tamanho do seg

    while(auxChar != '\n')
        auxChar=fgetc(synP); //Pula a lista de semáforos

    while(!feof(synP)) {
        fscanf(synP, "%s", comando);
        if(strcmp("exec", comando) == 0 || strcmp("print", comando) == 0){//a ideia é verificar se ele encaixa em alguma das funções, pra depois ler o int e fazer a operação
            fscanf(synP, " %d\n", &valorComando);
            pg->tempo+=valorComando;
        }

        else if(strcmp("P(s)", comando) == 0 || strcmp("P(t)", comando) == 0 || strcmp("V(t)", comando) == 0 || strcmp("V(s)", comando) == 0){
            getc(synP);
        }

        else
            fscanf(synP, " %d\n", &aux);
    }

    fclose(synP);
    return 1;
}


//Algumas funções do escalonamento

void SemaphoreP() {

}

void SemaphoreV() {
    
}