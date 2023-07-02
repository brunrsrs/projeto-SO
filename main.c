#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "functions.h"

    //Feito por Bruno ROdrigues da Silveira e Gabriel Inagaki Marcelino

//Defines
#define GIGA 1000000000 //o limite de memória é de 1 GB
#define nBlocos 125000 //cada bloco tem 8 kbytes, então cabem 125 mil de blocos nisso

//Variáveis globias
blocos bloco[nBlocos]; //vetor de blocos que irão armazenar os processos em páginas
char comando[10]; //string com o comando a ser lido dos programas (exec, read, write, etc)
int active = 0; //variavel que verifica se há programas sendo lidos para mostrar ou não no "case 2"
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //inicializa o bloqueador de thread

bcp b; //bloco de Controle de Processos
bcp lista; //bloco de controle pra E/S
bcp espera; //bloco pra espera dos semaforos

programa auxPg; //variavel global auxiliar para armazenar dados
semaforo sema[256]; //vetor com todos os semáforos possíveis
int funcAdicionadas = 0; //funçoes presentes na fila + a(s) ativa(s)
int i = 0;
int flag = 0;
int qtdEspera = 0;
char libSemaforo = 'a';

//Função main
int main()  {
    setlocale(LC_ALL, ""); //permite usar ç ã é ï ò û
    int op = 0;
    programa pg;
    char nomeProcesso[16];

    if (!inicializaBCP(&b)) //inicializa o BCP
        return 0;

    if (!inicializaBCP(&espera)) //inicializa a lista de espera
        return 0;

    if (!inicializaBCP(&lista)) //inicializa a lista de E/S
        return 0;

    if (!inicializaPg(&pg)) //inicializa o processo programa
        return 0;

    inicializaBlocos(bloco); //inicializa o vetor global de blocos (que tem cento e vinte e cinco mil de posições)

    pthread_t execThread; //função principal que vai rodar o tempo todo
    pthread_create(&execThread, NULL, exec, (void*)&b); //cria uma thread pro bcp

    pthread_t discThread;   //função para simular entrada e saída
    pthread_create(&discThread, NULL, funcES, (void*)&lista);

    pthread_t waitingThread; //função para espera de semaforos
    pthread_create(&waitingThread, NULL, wait, (void*)&espera);

        do {
            menu();
            scanf(" %d", &op);
            switch (op) {
                case 0:
                    break;
                case 1: //Caso inserir um arquivo
                    printf("\nInsira o nome do processo que deseja abrir: ");
                    scanf(" %s", nomeProcesso);
                    pthread_mutex_lock(&lock); //trava a fila para inserção
                    
                    processCreate(nomeProcesso);

                    pthread_mutex_unlock(&lock); //destrava
                    break;

                case 2: //Caso que mostra o status do executavel
                    if (active == 1) {
                        pthread_mutex_lock(&lock);
                        printf("\n\tStatus:");
                        printf("\nNome: %s", auxPg.nome);
                        printf("\nFunção executando: %s", comando);
                        printf("\nTempo estimado do processo: %d Unit. Tempo", auxPg.tempo);
                        printf("\nFunções na fila: %d\n", funcAdicionadas-1);
                        pthread_mutex_unlock(&lock);
                    }
                    else 
                        printf("\nNenhum programa execurando no momento!\n");
                    
                    break;

                case 3: //Caso que mostra a memória
                    pthread_mutex_lock(&lock);

                    printf("\n\tStatus da memória:");
                    printf("\nMemória livre: %d Kbytes", GIGA - b.tamTotal);
                    printf("\nPáginas livres: %d\n\n", (GIGA - b.tamTotal)/8000);
                    
/*                //PERIGO! - mostra a memória inteira (cento e vinte e cinto mil de espaços)
                    printf("\nMemória");    
                    for (i=0; i<nBlocos; i++) {
                        printf("%d ", bloco[i].ocupado);
                        if (bloco[i].paginas!=NULL)
                            printf("%d %d", bloco[i].paginas->numPag, bloco[i].paginas->refBit);
                        printf("|");
                    }
*/
                    pthread_mutex_unlock(&lock);
                    break;

                default: printf("Inválido"); 
                    break;
            }
        } while(op != 0);

pthread_mutex_destroy(&lock);

return 0;
}