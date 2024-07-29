#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define FILE_NAME "in.txt"

typedef struct Lista{
    unsigned int value;
    struct Lista *next;
} Lista;

Lista *L2 = NULL, *L3 = NULL;

void* l2_runner(void *param);
void* l3_runner(void *param);
void* l3_print_runner(void *param);

int main(){
    Lista *L1 = NULL, *auxNode, *newNode;

    pthread_t tid[3];
    pthread_attr_t attr;

    FILE *fp;
    unsigned int val;

    pthread_attr_init(&attr);

    /* Cria as threads */
    pthread_create(&tid[0], &attr, l2_runner, L1);
    pthread_create(&tid[1], &attr, l3_runner, NULL);
    pthread_create(&tid[2], &attr, l3_print_runner, NULL);

    fp = fopen(FILE_NAME, "r");

    if(!fp){
        perror("Error opening in.txt file!\n");
        return -1;
    }

    /* Lê os valores do arquivo txt um por um inserindo na L1 */
    while(fscanf(fp, "%1u", &val) != EOF){
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
    }

    /* Como o EOF descarta o último valor lido, insiro ele aqui */
    newNode = malloc(sizeof(Lista));
    newNode->value = val;
    newNode->next = NULL;

    auxNode->next->next = newNode;

    fclose(fp);

    return 0;
}

/* Lê a L1 e coloca em L2 os valores que não são maiores que 2 e pares */
void* l2_runner(void *param){
    Lista *L1 = (Lista*)param;

    while(L1->next)
        L1 = L1->next;

    if(!(L1->value > 2 && L1->value % 2 == 0)){
        Lista *newNode = malloc(sizeof(Lista));

        newNode->value = L1->value;
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

    pthread_exit(0);
}

/* Analisa os valores de L2 e se, o valor não for primo, insere em L3 */
void* l2_runner(void *param){
    Lista *auxNode = L2;
    unsigned int result = 0;

    while(auxNode->next)
        auxNode = auxNode->next;

    for (int i = 2; i <= auxNode->value / 2; i++) {
        if (auxNode->value % i == 0) {
            result++;
            break;
        }
    }

    if(result > 0){
        Lista *newNode = malloc(sizeof(Lista));

        newNode->value = auxNode->value;
        newNode->next = NULL;

        if(!L3)
            L3 = newNode;
        else{
            Lista *auxNode = L3;

            while(auxNode->next)
                auxNode = auxNode->next;

            auxNode->next = newNode;
        }
    }

    pthread_exit(0);
}

/* Imprime L3 */
void* l3_print_runner(void *param){
    Lista *auxNode = L3;

    while(auxNode->next){
        printf("%u ", auxNode->value);
        auxNode = auxNode->next;
    }

    pthread_exit(0);
}