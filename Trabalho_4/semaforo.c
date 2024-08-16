#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

typedef struct Lista{
    unsigned int value;
    struct Lista *next;
} Lista;

Lista *L1 = NULL, *L2 = NULL, *L3 = NULL;

#define SEM1_NAME "LISTA_1"
#define SEM2_NAME "LISTA_2"
#define SEM3_NAME "LISTA_3"
#define SEM4_NAME "TERMINATE"

sem_t *sem1 = NULL;
sem_t *sem2 = NULL;
sem_t *sem3 = NULL;
sem_t *sem4 = NULL;

void* l2_runner(void *param);
void* l3_runner(void *param);
void* l3_print_runner(void *param);

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: ./a.out <file_name>\n");
        return -1;
    }

    sem_unlink(SEM1_NAME);
    sem_unlink(SEM2_NAME);
    sem_unlink(SEM3_NAME);
    sem_unlink(SEM4_NAME);

    sem1 = sem_open(SEM1_NAME, O_CREAT | O_EXCL, 0664, 0);
    sem2 = sem_open(SEM2_NAME, O_CREAT | O_EXCL, 0664, 1);
    sem3 = sem_open(SEM3_NAME, O_CREAT | O_EXCL, 0664, 1);
    sem4 = sem_open(SEM4_NAME, O_CREAT | O_EXCL, 0664, 0);

    if (sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED || sem4 == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    char *FILE_NAME = argv[1];

    Lista *auxNode, *newNode;

    pthread_t tid[3];
    pthread_attr_t attr;

    FILE *fp;
    unsigned int val;

    pthread_attr_init(&attr);
    
    /* Cria as threads */
    pthread_create(&tid[0], &attr, l2_runner, NULL);
    pthread_create(&tid[1], &attr, l3_runner, NULL);
    // pthread_create(&tid[2], &attr, l3_print_runner, NULL);

    fp = fopen(FILE_NAME, "a");

    if(!fp){
        perror("Error opening in.txt file!\n");
        return -1;
    }

    fwrite("-1", sizeof(char), 1, fp);
    fclose(fp);

    fp = fopen(FILE_NAME, "r");

    /* Lê os valores do arquivo txt um por um inserindo na L1 */
    while(fscanf(fp, "%u", &val) != EOF){
        // printf("waiting for sem lock Thread 1\n");
        sem_wait(sem2);
        // printf("Acquired sem lock Thread 1\n");

        newNode = malloc(sizeof(Lista));
        newNode->value = val;
        newNode->next = NULL;

        if(!L1)
            L1 = newNode;
        else{
            auxNode = L1;
            
            while(auxNode->next)
                auxNode = auxNode->next;

            auxNode->next = newNode;
        }

        Lista *aux = L3;

        while(aux){
            printf("%u ", aux->value);
            aux = aux->next;
        }

        // printf("release sem lock Thread 1\n");
        sem_post(sem1);
    }

    fclose(fp);

    for(int i=0;i<2;i++)
        pthread_join(tid[i], NULL);

    sem_close(sem4);
    sem_unlink(SEM1_NAME);
    sem_unlink(SEM2_NAME);
    sem_unlink(SEM3_NAME);
    sem_unlink(SEM4_NAME);

    return 0;
}

/* Lê a L1 e coloca em L2 os valores que não são maiores que 2 e pares */
void* l2_runner(void *param){
    while(1){
        // printf("waiting for sem lock Thread 2\n");
        sem_wait(sem1);
        sem_wait(sem3);
        // printf("Acquired sem lock Thread 2\n");
        
        Lista *auxL1 = L1;
        
        while(auxL1 && auxL1->next)
            auxL1 = auxL1->next;

        printf("\n");
        
        if(auxL1 && !(auxL1->value > 2 && auxL1->value % 2 == 0)){
            Lista *newNode = malloc(sizeof(Lista));

            newNode->value = auxL1->value;
            newNode->next = NULL;

            if(!L2)
                L2 = newNode;
            else{
                Lista *auxNode = L2;

                while(auxNode->next)
                    auxNode = auxNode->next;

                auxNode->next = newNode;
            }
        }

        // printf("release sem lock Thread 2\n");
        sem_post(sem2);
    }

    pthread_exit(0);
}

/* Analisa os valores de L2 e se, o valor não for primo, insere em L3 */
void* l3_runner(void *param){
    while(1){
        printf("waiting for sem lock Thread 3\n");
        sem_wait(sem2);
        printf("Acquired sem lock Thread 3\n");

        Lista *auxL2 = L2;
        unsigned int result = 0;

        while(auxL2 && auxL2->next)
            auxL2 = auxL2->next;

        if(auxL2){
            for (int i = 2; i <= auxL2->value / 2; i++) {
                if (auxL2->value % i == 0) {
                    result++;
                    break;
                }
            }
        }

        if(result > 0){
            Lista *newNode = malloc(sizeof(Lista));
            
            newNode->value = auxL2->value;
            newNode->next = NULL;

            if(!L3)
                L3 = newNode;
            else{
                Lista *auxL2 = L3;

                while(auxL2->next)
                    auxL2 = auxL2->next;

                auxL2->next = newNode;
            }
        }

        printf("release sem lock Thread 3\n");
        sem_post(sem3);
    }

    pthread_exit(0);
}

/* Imprime L3 */
void* l3_print_runner(void *param){
    Lista *auxL3 = L3;

    while(auxL3->next){
        printf("%u ", auxL3->value);
        auxL3 = auxL3->next;
    }

    pthread_exit(0);
}