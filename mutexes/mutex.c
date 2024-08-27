//Aluno: Gustavo Kermaunar Volobueff
//rgm: 47006
//Prof. Fabrício Sérgio de Paula
//Disciplina: Sistemas Operacionais
//Trabalho 5 - Semáforos Binários

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct Lista{
    pthread_mutex_t mutex;
    int value;
    struct Lista *next;
} Lista;

Lista *L = NULL;

void* remove_even_runner();
void* remove_prime_runner();
void* print_runner();

void destroy_list();

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: ./a.out <file_name>\n");
        return -1;
    }

    const char *FILE_NAME = argv[1];
    FILE *fp;

    int val;

    pthread_t tid[3];
    pthread_attr_t attr;

    Lista *l_aux = NULL, *new_node = NULL;

    pthread_attr_init(&attr);

    /* Cria as threads */
    pthread_create(&tid[0], &attr, remove_even_runner, NULL);
    pthread_create(&tid[1], &attr, remove_prime_runner, NULL);
    pthread_create(&tid[2], &attr, print_runner, NULL);

    fp = fopen(FILE_NAME, "r");

    /* Lê o arquivo e insere no final da lista L os valores e seta os mutexes */
    while(fscanf(fp, "%d", &val) != EOF){
        new_node = malloc(sizeof(Lista));
        new_node->next = NULL;
        new_node->value = val;
        pthread_mutex_init(&(new_node->mutex), NULL);

        if(!L)
            L = new_node;
        else{
            l_aux = L;

            while(l_aux->next)
                l_aux = l_aux->next;

            l_aux->next = new_node;
        }
    }

    fclose(fp);

    for(int i=0;i<3;i++)
        pthread_join(tid[i], NULL);

    destroy_list();

    return 0;
}

/* Runner para remover os números pares maiores que 2 */
void* remove_even_runner(){
    Lista *l_aux = L;

    do{
        if(l_aux && !(l_aux->value > 2 && l_aux->value % 2 == 0)){
            if(l_aux == L){
                L = l_aux->next;
                free(l_aux);
            }
            else{
                Lista *prev = L;

                while(prev->next != l_aux)
                    prev = prev->next;

                prev->next = l_aux->next;
                free(l_aux);
            }
        }

        l_aux = l_aux->next;
    }while(l_aux);

    pthread_exit(0);
}

/* Runner para remover os não primos */
void* remove_prime_runner(){
    Lista *l_aux = L;

    do{
        unsigned int result = 0;
        
        if(l_aux && (l_aux->value == 0 || l_aux->value == 1))
            result = 1;

        if(l_aux && result == 0){
            for (int i = 2; i <= l_aux->value / 2; i++) {
                if (l_aux->value % i == 0) {
                    result++;
                    break;
                }
            }
        }

        if(l_aux && result == 0){

        }

        l_aux = l_aux->next;
    }while(l_aux);

    pthread_exit(0);
}

/* Runner para imprimir a lista */
void* print_runner(){
    Lista *l_aux = L;

    do{
        printf("%d ", l_aux->value);
        l_aux = l_aux->next;
    }while(l_aux);

    pthread_exit(0);
}

/* Desaloca a lista e destrói os mutex */
void destroy_list(){
    Lista *l_aux = L;

    while(l_aux){
        pthread_mutex_destroy(&(L->mutex));

        l_aux = l_aux->next;
        free(L);
        L = l_aux;
    }
}