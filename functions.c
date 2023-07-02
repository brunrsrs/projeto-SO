#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "functions.h"

#define GIGA 1000000000 //o limite é de memória é 1 GB
#define nBlocos 125000   //cada bloco tem 8 bytes, então cabem 125 mil de blocos nisso

//variaveis globais 2
extern semaforo sema[256]; 
extern int funcAdicionadas;
extern bcp b;
extern bcp lista;
extern bcp espera;
extern programa auxPg;
extern char comando[20];
extern int active;
extern blocos bloco[nBlocos];
extern pthread_mutex_t lock;
extern int flag; //verificação dos semáforos
extern int qtdEspera;
extern char libSemaforo;

//menu com opções
void menu() {
    printf("\n[1] Adicionar Thread\n[2] Detalhes do processo\n[3] Estado memória\n[0] Sair\nComando: ");
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
    p->pagina=NULL;
    p->posicao=0;
    p->qualSemaforo = 'a';
    return 1;
}

//inicializa os blocos
void inicializaBlocos(blocos *bloco){
    for(int i=0 ; i<nBlocos ; i++){
        bloco[i].ocupado=0;
        bloco[i].paginas=NULL;
    }
}

//principal função que vai executar todas as leituras dos arquivos
void *exec(void* banco) {
    bcp *b = (bcp*)banco;
    char auxChar = '0';
    char auxString[10];
    int auxInt;
    FILE* reader;

    while(1) { //programa vai executar durante todo programa (vai terminar com o return 0 da main)
    
        if (b->prog) { //verifica se há algum item na cabeça da lista

                strcpy(comando, "Lendo dados"); //prompt resposta usuário
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

                fseek(reader, b->prog->posicao, SEEK_SET); //posiciona o leitor para continuar de onde parou
                
                while (!feof(reader)) {
                    fscanf(reader, "%s", comando);
                    printf("%s\n", comando);

                    if(strcmp("exec", comando) == 0){ //ele pediu pra executar por auxInt tempo
                        fscanf(reader, " %d\n", &auxInt);
                        while(auxInt>=0) {
                            usleep(1000);
                            auxInt--;
                            auxPg.tempo--;
                        }
                    }
                    else if(strcmp("read", comando) == 0){
                        b->prog->posicao = ftell(reader);

                        pthread_mutex_lock(&lock);  
                        inserir(b->prog, &lista);
                        pthread_mutex_unlock(&lock);

                        fseek(reader, 0, SEEK_END);
                        getc(reader);
                    }
                    else if(strcmp("write", comando) == 0){
                        b->prog->posicao = ftell(reader);

                        pthread_mutex_lock(&lock);  
                        inserir(b->prog, &lista);
                        pthread_mutex_unlock(&lock);

                        fseek(reader, 0, SEEK_END);
                        getc(reader);
                    }
                    else if(strcmp("print", comando) == 0){ //ele pediu pra executar por auxInt tempo
                        fscanf(reader, " %d\n", &auxInt);
                        while(auxInt>=0) {
                            usleep(1000);
                            auxInt--;
                            auxPg.tempo--;
                        }
                    }

                    else if (comando[0] == 'P') {
                        for (auxInt=0; auxInt < strlen(auxPg.semaforos); auxInt++) {
                            if (auxPg.semaforos[auxInt] == comando[2])
                                if (semaphoreP(&sema[(int)auxPg.semaforos[auxInt]]) == 0) {
                                    getc(reader);
                                    b->prog->posicao = ftell(reader);
                                    b->prog->qualSemaforo = auxPg.semaforos[auxInt];
                                    pthread_mutex_lock(&lock);  

                                    inserir(b->prog, &espera);
                                    qtdEspera++;
                                    
                                    pthread_mutex_unlock(&lock);
                                    fseek(reader, 0, SEEK_END);
                                }
                        }
                        getc(reader);
                    }
                    else if (comando[0] == 'V') {
                        for (auxInt=0; auxInt < strlen(auxPg.semaforos); auxInt++) {
                            if (auxPg.semaforos[auxInt] == comando[2]) {
                                libSemaforo = auxPg.semaforos[auxInt];
                                semaphoreV(&sema[auxPg.semaforos[auxInt]]);
                            }
                        }
                        getc(reader);
                    }

                    else 
                        getc(reader);
                }
            fclose(reader);
            
            pthread_mutex_lock(&lock);

            if (!processFinish(b->prog->tamanho, b))
                break;
            funcAdicionadas--;

            pthread_mutex_unlock(&lock);        
        }
    active=0;
    sleep(1);
    }
}
//funções dos programas
//print e exec não têm funções dedicadas por não envolverem e/s, a ideia é usar esses em threads
void* funcES(void* lista){//previamente eram duas funções (funcRead e funcWrite), mas elas eram identicas
    FILE* reader;
    int tempo;
    bcp *l = (bcp*)lista;

    while(1){
        if(l->prog){
            programa *pg;
            pg = malloc(sizeof(programa));
            reader = fopen(l->prog->nome, "r");
            fseek(reader, l->prog->posicao, SEEK_SET);
            fscanf(reader, " %d", &tempo);
            getc(reader);
            l->prog->posicao = ftell(reader);

            usleep(1000 * l->prog->tempo);
            pg = l->prog;

            inserir(pg, &b);
            processFinish(l->prog->tamanho, l);
        }
    }
    printf("hino do palmeiras");
}

void *wait (void* listaEspera) { //lista de programas esperando pelo semaforo
    bcp* w = (bcp*)listaEspera;
    programa *auxProg = NULL;
    programa *auxAnterior = NULL;
    int flagAlt = 0; //flag para saber se deve mudar para próxima posição

    while (1) {
        if (w->prog) {
            if (flag == 1) { //se algum semaforo tiver sido liberado
                pthread_mutex_lock(&lock);
                auxAnterior = w->prog;
                auxProg = w->prog;
                do {
                    if (auxProg->prox && flagAlt == 0) {
                        auxProg = auxProg->prox;
                        flagAlt = 1;
                    }

                    if (auxAnterior->qualSemaforo == libSemaforo || auxProg->qualSemaforo == libSemaforo) { //for igual ao semaforo liberado
                            programa *auxRemov = NULL;
                            auxRemov = malloc(sizeof(programa));
                        
                        //remoção dessa lista
                        if (w->prog == auxAnterior) { //começo da lista
                            w->prog->prox = auxAnterior->prox;
                            auxRemov = auxAnterior;
                            auxRemov->prox = NULL;
                            inserir(auxRemov, &b);
                        }
                        
                        else if(!auxProg->prox && auxProg->qualSemaforo == libSemaforo) { //está no fim
                            auxAnterior->prox = NULL;
                            auxRemov = auxProg;
                            auxRemov->prox = NULL;
                            inserir(auxRemov, &b);
                        }

                        else { //está no meio
                            auxRemov = auxProg;
                            auxRemov->prox = NULL;
                            if (auxProg->prox)
                                auxProg = auxProg->prox;
                            auxAnterior->prox = auxProg;
                            inserir(auxRemov, &b);
                            flagAlt = 1;
                        }

                        qtdEspera--;
                    }
                    if (flagAlt == 0)
                        auxAnterior = auxProg;
                } while (auxProg->prox);

                pthread_mutex_unlock(&lock);
            }
        }
    }
}


//funções lista
int inserir(programa *prog, bcp *b){   //p é o programa a ser inserido, b é a bcp mesmo
    if(b->tamTotal+prog->tamanho <= GIGA) {
        b->tamTotal+=prog->tamanho;
        atribuiPagina(prog, prog->tamanho);

    //cria um programa auxiliar que será adicionado a lista
    programa *p = NULL;
    p = malloc(sizeof(programa));
    p = prog;

        if (!p || !b) {
            printf("\n\tNão foi possível inicializar\n");
            return 0;
        }

        //insere vazia
        if(b->prog == NULL) {
            b->prog = p;
            b->prog->prox = NULL;
            funcAdicionadas++;
            return 1;
        }

        programa *aux;

        //caso precise inserir no começo, inserir na 2° posição para não quebrar o que está rodando
        if (p->tempo < b->prog->tempo) {
            if (!b->prog->prox) { //caso não tenha mais nada depois
                b->prog->prox = p;
                p->prox = NULL;
                funcAdicionadas++;
                return 1;
            }
            p->prox = b->prog->prox;
            b->prog->prox = p;
            funcAdicionadas++;
            return 1;
        }

        programa *proxAux;  //auxiliar que será sempre prox do auxiliar anterior
        proxAux=NULL;   //Inicializando variavel

        if (b->prog->prox)  //Caso exista um 2° elemento
            proxAux = b->prog->prox;
        
        aux = b->prog;  //variavel anterior ao proxAux
//esse while vai falhar caso exista apenas 1 elemento na fila ou chegar ao fim ou encontrar o lugar de inserir
        while (proxAux && proxAux->tempo < p->tempo) {
            aux = aux->prox;
            if (proxAux->prox)
                proxAux = proxAux->prox;
            else
                proxAux = NULL;

        funcAdicionadas++;
        return 1;
        }

        //insere no final (caso tenha apenas 1 elemento ou vários)
        if (!aux->prox) {
            aux->prox = p;
            p->prox = NULL;
            funcAdicionadas++;
            return 1;
        }

        //insere no meio
        p->prox = aux->prox;
        aux->prox = p;
        funcAdicionadas++;
        return 1;
    }
    else{
        printf("\n\tO limite máximo de armazenamento foi atingido\n");
        return 0;
    }
}

//funções pras páginas
void atribuiPagina(programa *p, float tam){ //ocupa o (teto do tamanho ocupado/8) páginas
    int paginaIndex=-1;// variável para armazenar o índice da página no bloco, se permanecer -1, é que nao achou espaço disponivel
    p->pagina = malloc((ceil(tam/8.0)) * sizeof(int));    //vai precisar de um vetor dinamico pra guardar as paginas ocupadas
    for (int i = 0; i<(ceil(tam/8.0)); i++)
        p->pagina[i] = 0;

    for(int i=0 ; i < ceil(tam/8.0) ; i++){
        for(int j=0 ; j<nBlocos ; j++){
            if (bloco[j].ocupado == 0 || bloco[j].paginas == NULL || bloco[j].paginas->refBit == 0){//verificando se ta disponivel antes de colocar no espaço
                bloco[j].ocupado = 1;
                bloco[j].paginas = malloc(sizeof(pagina)); // aloca a estrutura pagina para a página atual
                bloco[j].paginas->refBit = 1;
                bloco[j].paginas->numPag = i;
                p->pagina[i] = bloco[j].paginas->numPag;//se um processo ocupa 1 pagina, ele terá a pagina[0] atrelado à pagina 20 da memória, por exemplo
                paginaIndex = j;
                break;
            }
            else bloco[j].paginas->refBit = 0; //Reseta o bit de referência
        }

        if (paginaIndex == -1) {//segunda chance
            while (paginaIndex == -1) { //caso nao entre no if dentro do for j
                for (int j = 0; j < nBlocos; j++) {
                    if (bloco[j].paginas == NULL || bloco[j].paginas->refBit == 0) {
                        bloco[j].ocupado = 1;
                        bloco[j].paginas = malloc(sizeof(pagina));
                        bloco[j].paginas->refBit = 1;
                        bloco[j].paginas->numPag = i;
                        p->pagina[i] = bloco[j].paginas->numPag;
                        paginaIndex = j;
                        break;
                    } 
                    else bloco[j].paginas->refBit = 0; // reseta o bit de referência
                }
            }
        }
    }
}


void removePagina(bcp *b, float tam) {//remove as paginas a partir do endereço físico delas, dado no vetor 'pagina' do programa
    int pageIndex;
    int i;
    for (i = 0; i < ceil(tam / 8.0); i++) {
        pageIndex = b->prog->pagina[i]; //pra nao ter sobrecarga de notação
        bloco[pageIndex].ocupado = 0;

        if (bloco[pageIndex].paginas != NULL) {
            free(bloco[pageIndex].paginas);
            bloco[pageIndex].paginas = NULL;//deu mais certo assim
        }
    }
}

//para remover um processo quando seu tempo zera
int processFinish(int tam, bcp *b){ 

    if (!b || !b->prog) {
        printf("\nAlgo deu errado no processo");
        return 0;
    }

    if(b->tamTotal - tam < 0){   //verificando se a subtração dá um valor inválido
        printf("\nAlgo deu errado na remoção");
        return 0;
    }
    programa *aux = b->prog;
    if (aux->pagina != NULL)
        removePagina(b, tam);
    
    if(aux->prox) //como a inserção é ordenada, a remoção será sempre no início da lista
        b->prog = aux->prox;
    
    else 
        b->prog = NULL;   //quando nao resta nada depois da remoção
    
    b->tamTotal -= tam;
    free(aux);
    return 1;
}

int processCreate(char nomeProcesso[10]){ //synP de synthethic program
    FILE *synP;
    char comando[5];
    int valorComando;
    char auxTxt[10];
    char auxChar;
    int aux;
    programa *pg;
    pg = malloc(sizeof(programa));

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
        auxChar=fgetc(synP); //pula lista de semaforos

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

    if (inserir(pg, &b))
        printf("\nProcesso Inserido\n");
    return 1;
}


//Algumas funções do escalonamento
int semaphoreP(semaforo *s) {
    if (s->valor == 0) { //verifica se está sendo utilizada
        s->valor = 1;
        return 1;
    }

    else
        return 0;

}

int semaphoreV(semaforo *s) {
    if (s->valor == 1) { //verifica se está sendo utilizado
        s->valor = 0;
        flag = 1;
        return 1;
    }

    else {
        printf("\nSemaforo não ocupado\n");
        return 0;
    }
}