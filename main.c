#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <pthread.h>
#include <semaphore.h>
#include "functions.h"

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
volatile int t=0;

void* timer() {
    while(1) {
        t++;
    }
}

int main()  {
    setlocale(LC_ALL, ""); //permite usar ç ã é ï ò û
    int op = 0;

    char nomeProcesso[10];

    pthread_t t1;
    // pthread_create(&t1, NULL, (void*)função, argumentos); como criar uma thread aí
    // pthread_join(t1, NULL); como colocar a thread pra rodar

    if (pthread_create(&t1, NULL, timer, NULL) == 0) {
        do {
            menu();
            scanf(" %d", &op);
            switch (op) {
                case 0:
                    break;
                case 1:
                    printf("\nInsira o nome do processo que deseja abrir\n");
                    scanf(" %[^\n]c", nomeProcesso);
//                    programRead(nomeProcesso);//aaaaaaaaaaaaaaaaaaaaaa
                    break;
                case 2:
                    break;
                case 3:
                    break;
                default: printf("Inválido"); 
                    break;
            }
        } while(op != 0);
    }

    else
        printf("Erro ao criar o temporizador\n");

return 0;
}