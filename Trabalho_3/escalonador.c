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

    unsigned int quantCpuBurst;
    unsigned int quantIoBurst;
    unsigned int cpuBurstCounter;
    unsigned int ioBurstCounter;

    unsigned int processValue;

    unsigned int endTime;
    struct processQueue *next;
} processQueue;

void insertFileInQueue(char *archName, processQueue **queue, unsigned int *quantProcesses);


void swap(unsigned int *a, unsigned int *b);
void clearPointers(processQueue **queue);
void sortArray(unsigned int *ioBurstEnds, unsigned int quantIOsArray);

void FCFSAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses);

int main(int argc, char *argv[]){
    char *archName = argv[1];
    unsigned int quantum = atoi(argv[2]);
    unsigned int sequential = 0, quantProcesses=0;

    processQueue *queue = NULL;

    if(argc == 4 && strcpy(argv[3], "-seq") == 0)
        sequential++;

    insertFileInQueue(archName, &queue, &quantProcesses);

    /* Algoritmos fazem os escalonamentos e imprimem os resultados solicitados */
    printf("---------------\n1. Algoritmo FCFS\n---------------\n\n");
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
        newNode->quantCpuBurst = 0;
        newNode->quantIoBurst = 0;
        newNode->cpuBurstCounter = 0;
        newNode->ioBurstCounter = 0;
        newNode->processValue = (*quantProcesses) + 1;
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
                    if(atoi(token) != 0)
                        newNode->ioBursts[ioBurstCount++] = atoi(token);

                if(strcmp(token, "\n") == 0){
                    token = strtok(NULL, " ");
                    break;
                }
            }

            i++;    
        } while(token = strtok(NULL, " "));

        newNode->quantCpuBurst = cpuBurstCount;
        newNode->quantIoBurst = ioBurstCount;

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
    unsigned int currTime, finishedProcesses=0, ioBurstEnds[quantProcesses];
    unsigned int quantIOsArray=0, ioArrayCounter=0;
    processQueue *auxQueue = queue;
    
    printf("-> Diagrama de Gantt\n\n");
    printf("%u|", auxQueue->submission);
    currTime = auxQueue->submission;

    while(auxQueue){
        if(currTime >= auxQueue->submission) /* Caso o tempo corrente seja maior ou igual a submissão, ele segue normalmente */
            printf("P%u %u|", auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
        else{ /* Caso o tempo corrente seja menor que o tempo de submissão, haverá um tempo ocioso no cpu */
            currTime = auxQueue->submission;
            printf("*** %u|P%u %u|", currTime, auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
        }

        currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
        auxQueue->endTime = currTime;
        
        /* Array para guardar as E/S dos processos, caso possuam */
        if(auxQueue->quantIoBurst > 0)
            ioBurstEnds[quantIOsArray++] = currTime + auxQueue->ioBursts[auxQueue->ioBurstCounter];
        else
            finishedProcesses++;

        auxQueue = auxQueue->next;
    }

    while(finishedProcesses < quantProcesses){
        sortArray(ioBurstEnds, quantIOsArray);

        auxQueue = queue;

        /* Acha o processo que encerrará primeiro */
        while(auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter] != ioBurstEnds[ioArrayCounter])
            auxQueue = auxQueue->next;

        if(auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter] <= currTime)
            printf("P%u %u|", auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
        else{
            currTime = auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter];
            printf("*** %u|P%u %u|", currTime, auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
        }

        auxQueue->ioBurstCounter++;
        currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
        auxQueue->endTime = currTime;
        
        if(auxQueue->ioBurstCounter < auxQueue->quantIoBurst)
                        

    }
    
    // /* Aqui os processos são colocados em submissão */
    // while(auxQueue){
    //     if(currTime >= auxQueue->submission)
    //         printf("P%u %u|", ++processCount, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
    //     else{
    //         currTime = auxQueue->submission;
    //         printf("*** %u|P%u %u|", auxQueue->submission, ++processCount, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
    //     }

    //     currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
    //     auxQueue->endTime = currTime;

    //     if(auxQueue->ioBursts[0]){
    //         ioEndsPos[processCount-1] = processCount-1;
    //         ioBurstEnds[processCount-1] = auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter++];
    //         quantIOs++;
    //     }
    //     else    
    //         finishedProcesses++;

    //     auxQueue = auxQueue->next;
    // }

    // unsigned int posToSeeArray, quantIOsAux;

    // if(!seq){
    //     while(finishedProcesses != quantProcesses){
    //         for(int i=1;i<quantIOs;i++){
    //             for(int j=0;j<quantIOs - i;j++){
    //                 if(ioBurstEnds[j] > ioBurstEnds[j+1]){
    //                     swap(&ioBurstEnds[j], &ioBurstEnds[j+1]);
    //                     swap(&ioEndsPos[j], &ioEndsPos[j+1]);
    //                 }
    //             }
    //         }

    //         posToSeeArray=0;
    //         quantIOsAux=0;

    //         while(posToSeeArray != quantIOs){
    //             auxQueue = queue;
    //             processCount=0;

    //             while(processCount != ioEndsPos[posToSeeArray]){
    //                 auxQueue = auxQueue->next;
    //                 processCount++;
    //             }

    //             if(ioBurstEnds[posToSeeArray] > currTime){
    //                 currTime = ioBurstEnds[posToSeeArray];
    //                 printf("*** %u|P%u %u|", currTime, processCount+1, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
    //                 currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
    //             }
    //             else{
    //                 printf("P%u %u|", processCount+1, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
    //                 currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
    //             }

    //             auxQueue->endTime = currTime;

    //             if(auxQueue->ioBursts[auxQueue->ioBurstCounter]){
    //                 ioEndsPos[processCount] = processCount;
    //                 ioBurstEnds[processCount] = currTime + auxQueue->ioBursts[auxQueue->ioBurstCounter++];
    //                 quantIOsAux++;
    //             }

    //             if(!(auxQueue->cpuBursts[auxQueue->cpuBurstCounter]))
    //                 finishedProcesses++;

    //             posToSeeArray++;
    //         }
            
    //         quantIOs = quantIOsAux;
    //     }
    // }

}

void sortArray(unsigned int *ioBurstEnds, unsigned int quantIOsArray){
    for(int i=1;i<quantIOsArray;i++)
        for(int j=0;j<quantIOsArray - i;j++)
            if(ioBurstEnds[j] > ioBurstEnds[j+1])
                swap(&ioBurstEnds[j], &ioBurstEnds[j+1]);
        
}

void swap(unsigned int *a, unsigned int *b){
    unsigned int aux = *a;
    *a = *b;
    *b = aux;
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