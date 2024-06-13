/* Aluno: Gustavo Kermaunar Volobueff rgm47006
   Disciplina de Sistemas Operacionais
   Professor Doutor Fábricio Sérgio de Paula */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Struct que define a lista encadeada de processos */
typedef struct processQueue{
    unsigned int priority;
    unsigned int submission;
    unsigned int *bursts;
    struct processQueue *next;
} processQueue;

void insertFileInQueue(char *archName, processQueue **queue);
void printInformations();
void FCFSAlgorithm(processQueue *queue);

void clearPointers(processQueue **queue);

int main(int argc, char *argv[]){
    char *archName = argv[1];
    unsigned int quantum = atoi(argv[2]);
    unsigned int sequential = 0;

    processQueue *queue = NULL;

    if(argc == 4 && strcpy(argv[3], "-seq") == 0)
        sequential++;

    insertFileInQueue(archName, &queue);

    /* Algoritmos fazem os escalonamentos e imprimem os resultados solicitados */
    FCFSAlgorithm(queue);

    

    clearPointers(&queue);

    return 0;
}

/* Faz a leitura do arquivo em uma lista encadeada de processos */
void insertFileInQueue(char *archName, processQueue **queue){
    FILE *fp = fopen(archName, "rb");

    if(!fp){
        printf("File <%s> not found!\n", archName);
        exit(-1);
    }

    /* Tamanho do arquivo */
    fseek(fp, 0L, SEEK_END); 
    unsigned int size = ftell(fp);
    rewind(fp);
    
    char *contentsArray = malloc(size), auxChar;
    int i=0;

    /* Faz a leitura do arquivo num array para salvar na lista depois, separando o final de cada linha para que
    a separação dos tokens sejam identificados */
    while(fread(&auxChar, sizeof(char), 1, fp)){
        if(auxChar == '\n'){
            contentsArray[i++] = ' ';
            contentsArray[i++] = auxChar;
            contentsArray[i++] = ' ';
        }
        else
            contentsArray[i++] = auxChar;
    }

    fclose(fp);

    i=0;
    char *token = strtok(contentsArray, " ");
    unsigned int burstCount=0;

    /* Salva na lista os valores, separando prioridade, submissão e picos */
    while(1){
        processQueue *newNode = malloc(sizeof(processQueue));
        newNode->bursts = malloc(sizeof(unsigned int) * 100);
        newNode->next = NULL;
        
        do{
            if(i == 0)
                newNode->priority = atoi(token);
            else if(i == 1)
                newNode->submission = atoi(token);
            else{
                newNode->bursts[burstCount++] = atoi(token);
                
                if(strcmp(token, "\n") == 0){
                    token = strtok(NULL, " ");
                    break;
                }
            }

            i++;    
        } while(token = strtok(NULL, " "));
        
        /* Salva na lista encadeada o novo nó criado do processo */
        if(!(*queue))
            *queue = newNode;
        else{
            processQueue *queueAux = *queue;

            while(queueAux->next)
                queueAux = queueAux->next;

            queueAux->next = newNode;
        }

        burstCount=0;
        i=0;

        if(!token)
            break;
    }

    free(contentsArray);
}

void FCFSAlgorithm(processQueue *queue){
    
}

void clearPointers(processQueue **queue){
    processQueue *aux = *queue;

    while((*queue)->next){
        free((*queue)->bursts);
        *queue = (*queue)->next;
        free(aux);
        aux = *queue;
    }
}