#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <pthread.h>
#include<semaphore.h>

//funções

int main()  {
    setlocale(LC_ALL, ""); //permite usar ç ã é ï ò û
    int op = 0;

    do {
        printf("[1] Adicionar thread\n[2] Listar threads\n[0] Sair\nComando: ");
        scanf(" %d", &op);
        switch (op) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            default: printf("Inválido"); 
                break;
        }
    } while(op != 0);

return 0;
}