typedef struct programa {
    char nome[20];
    int ident;
    int prior;
    char semaforos[10];
    int tamanho;
    int tempo;
    struct programa *prox; //próximo da lista
} programa;

typedef struct bcp {
    programa *prog; //cabeça da lista
    int tamTotal;
} bcp;

//structs pra paginação
typedef struct blocos{
    int ocupado;
    struct pagina *pagina;
} blocos;

typedef struct pagina{
    int refBit;
} pagina;



//funções de controle
void menu();
int inicializaBCP(bcp*);
int inicializaPg(programa*);
void *exec();

//funções da lista
int inserir(struct programa*, struct bcp*);
int processFinish(int, struct bcp*); //previamente conhecida como "remover"

//funções do banco de dados
int programRead(struct programa*, char*, struct bcp*);
void processCreate(struct programa*, struct bcp*);
void memLoadReq(struct programa*, struct bcp*);
int interruptControl(); //retorna valor que ativa a thread
void processInterrupt();
void memLoadFinish();


//funções do escalonador
void semaphoreP();  //TO DO
void semaphoreV();  //TO DO