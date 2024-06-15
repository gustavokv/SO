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
    unsigned int *cpuBursts;
    unsigned int *ioBursts;
    unsigned int cpuBurstCount;
    unsigned int ioBurstCount;
    unsigned int endTime;
    struct processQueue *next;
} processQueue;

void insertFileInQueue(char *archName, processQueue **queue, unsigned int *quantProcesses);
void printInformations();
void FCFSAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses);

void clearPointers(processQueue **queue);

int main(int argc, char *argv[]){
    char *archName = argv[1];
    unsigned int quantum = atoi(argv[2]);
    unsigned int sequential = 0, quantProcesses=0;

    processQueue *queue = NULL;

    if(argc == 4 && strcpy(argv[3], "-seq") == 0)
        sequential++;

    insertFileInQueue(archName, &queue, &quantProcesses);

    /* Algoritmos fazem os escalonamentos e imprimem os resultados solicitados */
    FCFSAlgorithm(queue, sequential, quantProcesses);

    

    clearPointers(&queue);

    return 0;
}

/* Faz a leitura do arquivo em uma lista encadeada de processos */
void insertFileInQueue(char *archName, processQueue **queue, unsigned int *quantProcesses){
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
    unsigned int cpuBurstCount=0, ioBurstCount=0;

    /* Salva na lista os valores, separando prioridade, submissão e picos */
    while(1){
        processQueue *newNode = malloc(sizeof(processQueue));
        newNode->cpuBursts = malloc(sizeof(unsigned int) * 100);
        newNode->ioBursts = malloc(sizeof(unsigned int) * 100);
        newNode->cpuBurstCount = 0;
        newNode->ioBurstCount = 0;
        newNode->next = NULL;
        
        do{
            if(i == 0)
                newNode->priority = atoi(token);
            else if(i == 1)
                newNode->submission = atoi(token);
            else{
                if(i % 2 == 0)
                    newNode->cpuBursts[cpuBurstCount++] = atoi(token);
                else
                    newNode->ioBursts[ioBurstCount++] = atoi(token);
                
                if(strcmp(token, "\n") == 0){
                    newNode->cpuBursts[cpuBurstCount++] = 0;
                    newNode->ioBursts[ioBurstCount++] = 0;
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

        (*quantProcesses)++;
        cpuBurstCount=0;
        ioBurstCount=0;
        i=0;

        if(!token)
            break;
    }

    free(contentsArray);
}

void FCFSAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses){
    unsigned int currTime, smallestIOCounter, processCount=1, finishedProcesses=0, cpuBurstEnds[quantProcesses][2];
    processQueue *auxQueue = queue;

    printf("%u[", auxQueue->submission);
    currTime = auxQueue->submission;
    smallestIOCounter = auxQueue->submission + auxQueue->cpuBursts[0] + auxQueue->ioBursts[0];
    
    /* Aqui os processos são colocados em submissão */
    while(auxQueue){
        if(currTime >= auxQueue->submission)
            printf("P%u %u|", processCount++, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCount]);
        else{
            currTime = auxQueue->submission;
            printf("*** %u|P%u %u|", auxQueue->submission, processCount++, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCount]);
        }

        currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCount++];
        auxQueue->endTime = currTime;
        
        cpuBurstEnds[processCount-1][0] = processCount-1;
        cpuBurstEnds[processCount-1][1] = auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCount];

        auxQueue = auxQueue->next;
    }
    
    if(!seq){
        while(finishedProcesses != quantProcesses){
            
        }
    }

}

void clearPointers(processQueue **queue){
    processQueue *aux = *queue;

    while((*queue)->next){
        free((*queue)->cpuBursts);
        free((*queue)->ioBursts);
        *queue = (*queue)->next;
        free(aux);
        aux = *queue;
    }
}