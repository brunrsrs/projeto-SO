#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "functions.h"

#define GIGA 1000000000 //o limite é de memória é 1 GB
#define nBlocos 125000000 //cada bloco tem 8 bytes, então cabem 125 milhões de blocos nisso

//funções

/*
Coisas a se fazer:

    Funções
.interruptControl
.sysCall

.processInterrupt
.semaphoreP
.semaphoreV 
.memLoadReq 
.memLoadFinish
.processCreate 
.processFinish  

    Algoritmo de Escalonamento
.Shortest Remaining Time First
    -determinar tempo de execução da função (soma tempos)

    Gerenciamento de memória
.Max 1 Gbyte
    -memória com alocação em blocos, com páginas de 8 kbytes
    -estrutura baseada em segmentos
    -memória virtual usando o algoritmo da segunda chance

*/

//Temporizador sintético do programa

//Variáveis globias
blocos bloco[nBlocos];  //tem que inicializar
char comando[10];
int active = 0;
pthread_mutex_t lock;
bcp b;
programa auxPg;

//Função main
int main()  {
    setlocale(LC_ALL, ""); //permite usar ç ã é ï ò û
    int op = 0;
    programa pg;
    char nomeProcesso[16];

    if (!inicializaBCP(&b))
        return 0;

    if (!inicializaPg(&pg))
        return 0;

    pthread_t execThread;
    pthread_create(&execThread, NULL, exec, (void*)&b);

        do {
            menu();
            scanf(" %d", &op);
            switch (op) {
                case 0:
                    break;
                case 1:
                    printf("\nInsira o nome do processo que deseja abrir\n");
                    scanf(" %s", nomeProcesso);
                    pthread_mutex_lock(&lock);
                    programRead(&pg, nomeProcesso, &b);
                    inserir(&pg, &b);
                    pthread_mutex_unlock(&lock);
                    break;
                case 2:
                    if (active == 1) {
                        pthread_mutex_lock(&lock);
                        printf("\n\tStatus:");
                        printf("\nNome: %s", auxPg.nome);
                        printf("\nFunção executando: %s", comando);
                        printf("\nTempo estimado do processo: %d U.T.\n", auxPg.tempo);
                        pthread_mutex_unlock(&lock);
                    }
                    else {
                        printf("\nNenhum programa execurando no momento!");
                    }
                    break;
                case 3:
                    printf("\n\tStatus da memória:");
                    sleep(3);
                    printf("\nSeila maluco nao fiz ainda kk\n");

                    break;
                default: printf("Inválido"); 
                    break;
            }
        } while(op != 0);

return 0;
}