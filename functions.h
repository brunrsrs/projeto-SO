typedef struct programa {
    char nome[20];
    int ident;
    int prioridade;
    int tamanho;//*
    int tempo;
    char semaforos[5];//coloquei esses 2 parametros aqui, de acordo com o gpt isso é o bcp já
} pg;

void menu();

int interruptControl(); //retorna valor que ativa a thread

void processInterrupt();

void memLoadReq(pg);

void memLoadFinish();

//void processCreate(pg);

void processFinish(pg);

void programRead(char);