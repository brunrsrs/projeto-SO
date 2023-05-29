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
blocos bloco[nBlocos];
char comando[10];
int active = 0;
pthread_mutex_t lock;
bcp b;

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
                    programRead(&pg, nomeProcesso, &b);
                    pthread_mutex_lock(&lock);
                    inserir(&pg, &b);
                    pthread_mutex_unlock(&lock);
                    break;
                case 2:
                    break;
                case 3:
                    break;
                default: printf("Inválido"); 
                    break;
            }
        } while(op != 0);

return 0;
}