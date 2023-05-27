#include <stdio.h>
#include "functions.h"

memoriaOcupada=0;

void menu() {
    printf("\n[1] - Adicionar Thread\n[2] Listar Threads\n[0] Sair\n[3] Estado memória\nComando: ");
}

void memLoadReq(pg processo) {
    // Simular a operação de carregamento do processo na memória
    printf("\nCarregando processo na memória...\n");
    
    memoriaOcupada+=processo.tamanho;   //memoria ocupada pode ser uma var global sem muitos problemas acho

    if(memoriaOcupada>1000000000)
        printf("\nMemória cheia, não é possível ocupar mais dela.\n");
    else memLoadFinish();
}

void memLoadFinish(){//essa daqui so sinaliza que terminou de carregar????
    printf("\nProcesso carregado na memória.\n");

    //em algum lugar dessas funções de carregar/encerrar vai ter que chamar o semáforo provavelmente, pra fechar de fato os "processos"
}

// Função para simular a criação de um processo no BCP
void processCreate(pg processo) {
    //a ideia é pegar os valores do header e criar o processo aqui, já atribuindo o thread dele e tudo mais
    //muito provavelmente vai ter que ter uma lista de objetos ou parecido pra adicionar mais um processo sempre que necessario
    memLoadReq(processo);
}

void processFinish(pg processo){
    printf("\nEncerrando processo...\n");
    memoriaOcupada-=processo.tamanho;
    printf("\nProcesso encerrado.\n");
}

void programRead(FILE *synP, pg programa, char nomeProcesso[10]){ //synP de synthethic program
    //vai ser necessária a leitura dos "programas" (arquivos), essa função é pra isso
    //acho que vai acabar tendo que criar um objeto por programa ou fazer um vetor deles (e fazer funcionar)
    char comando[5];
    int valorComando;

    synP = fopen("nomedoprocesso.txt", "r");    //nao consegui fazer abrir de acordo com o que o usuario escreve
    if (synP == NULL) {
        printf("\nErro ao abrir o programa\n");
        return 1;
    }

    //pega os valores do header do txt
    fscanf(synP, "%s", programa.nome);
    fscanf(synP, "%d", &programa.ident);
    fscanf(synP, "%d", &programa.prioridade);
    fscanf(synP, "%d", &programa.tamanho);
    fscanf(synP, "%[^\n]", programa.semaforos);

    void processCreate(programa);

    //pega os comandos do txt
    do{
        fscanf(synP, "%s", comando);
        printf("\nComando: %s\n", comando);//pra ver melhor
        if(comando=='exec'){//a ideia é verificar se ele encaixa em alguma das funções, pra depois ler o int e fazer a operação
            fscanf(synP, "%d", &valorComando);
            func_exec(valorComando);//o valor aqui é o tempo de execução
        }
        else if(comando=='read'){
            fscanf(synP, "%d", &valorComando);
            func_read(valorComando);//o valor aqui é a trilha a ser lida
        }
        else if(comando=='write'){
            fscanf(synP, "%d", &valorComando);
            func_write(valorComando);//o valor aqui é a trilha a ser escrita
        }
        else if(comando=='print'){
            fscanf(synP, "%d", &valorComando);
            func_print(valorComando);//o valor aqui o tempo que o programa imprimirá resultados(?)
        }
        else if(comando=='P(s)'){//acessa região crítica guardada por s
            semaphoreP(s);//tem que ver exatamente do que se trata 's' e 't'
        }
        else if(comando=='V(s)'){//libera região crítica guardada por s
            semaphoreV(s);
        }
        else if(comando=='V(t)'){
            semaphoreV(t);
        }
        else if(comando=='P(t)'){
            semaphoreP(t); 
        }
        else printf("\nO comando %s não é válido", comando);
    }while(comando!=NULL);
    

    //so pra ver os valores
    printf("Nome: %s\n", programa.nome);
    printf("Ident: %d\n", programa.ident);
    printf("Prioridade: %d\n", programa.prioridade);
    printf("Tamanho: %d\n", programa.tamanho);
    printf("Semaforos: %s\n", programa.semaforos);

    fclose(synP);
    return 0;
}