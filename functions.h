//Struct que armazenará os valores lidos
typedef struct programa {
    char nome[20];
    int ident;
    int prior;
    char semaforos[10];
    int tamanho;
    int tempo;
    float *pagina;
    struct programa *prox; //próximo da lista
} programa;

//Bloco de banco de dados
typedef struct bcp {
    programa *prog; //cabeça da lista
    int tamTotal;
} bcp;

//Struct da página da paginação
typedef struct pagina{
    int numPag;
    int refBit;
} pagina;

//structs pra paginação
typedef struct blocos{
    int ocupado;
    struct pagina *paginas;
} blocos;

typedef struct semaforo {
    int posiFila;
    struct programa *queue[10]; //Número arbitrário de placeholder
    int valor;
} semaforo;

//funções de controle
void menu();
int inicializaBCP(bcp*);
int inicializaPg(programa*);
void *exec(void*);

//funções do banco de dados
int processCreate(char*); //previamente conhecida como "progRead"
int inserir(struct programa*, struct bcp*);
int processFinish(int, struct bcp*); //previamente conhecida como "remover"

//funções de paginação
void inicializaBlocos(blocos*);
void atribuiPagina(struct programa*, float);
void removePagina(struct bcp*, float);

//funções do escalonador
void semaphoreP(semaforo *);
void semaphoreV(semaforo *);