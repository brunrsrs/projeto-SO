typedef struct programa {
    char nome[20];
    int ident;
    int prioridade;
    int tempo;
    int tamanho;
    char semaforos[5];//coloquei esses 2 parametros aqui, de acordo com o gpt isso é o bcp já
} programa;


typedef struct bcp {
    programa *prog;
    int tamTotal;
    struct bcp *prox;
} bcp;


//funções de controle
void menu();
int inicializaBCP(bcp*);

//funções do banco de dados
int programRead(struct programa*, char*, struct bcp*);
void processCreate(struct programa*, struct bcp*);
void memLoadReq(struct programa*, struct bcp*);
int interruptControl(); //retorna valor que ativa a thread
void processInterrupt();
void memLoadFinish();
void processFinish(struct programa, struct bcp);

//funções do escalonador