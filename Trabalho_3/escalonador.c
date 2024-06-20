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

    unsigned int waitTime;
    unsigned int turnAround;

    unsigned int endTime;
    struct processQueue *next;
} processQueue;

void insertFileInQueue(char *archName, processQueue **queue, unsigned int *quantProcesses);
void copyToFile(char *fileName, char *insert);

void swap(unsigned int *a, unsigned int *b);
void clearPointers(processQueue **queue);
void sortArray(unsigned int *array, unsigned int n);
void resetStruct(processQueue **queue);

void FCFSAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses, char *fileName);
void SJFAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses, char *fileName);

int main(int argc, char *argv[]){
    char *archName = argv[1], outFileName[strlen(argv[1])+4];
    unsigned int quantum = atoi(argv[2]);
    unsigned int sequential = 0, quantProcesses=0;

    processQueue *queue = NULL;

    if(argc == 4 && strcmp(argv[3], "-seq") == 0)
        sequential++;
    
    memcpy(outFileName, archName, strlen(archName) - 3);
    strcat(outFileName, ".out");
    remove(outFileName);

    insertFileInQueue(archName, &queue, &quantProcesses);

    /* Algoritmos fazem os escalonamentos e imprimem os resultados solicitados */
    copyToFile(outFileName, "------------------------------------------FCFS------------------------------------------\n\nDiagrama de Gantt: ");
    FCFSAlgorithm(queue, sequential, quantProcesses, outFileName);
    resetStruct(&queue);
    copyToFile(outFileName, "\n\n------------------------------------------SJF------------------------------------------\n\nDiagrama de Gantt: ");
    SJFAlgorithm(queue, sequential, quantProcesses, outFileName);
    resetStruct(&queue);

    

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
        newNode->waitTime = 0;
        newNode->turnAround = 0;
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

void FCFSAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses, char *fileName){
    unsigned int currTime, finishedProcesses=0, ioBurstEnds[quantProcesses], ioBurstEndsAux[quantProcesses];
    unsigned int quantIOsArray=0, ioArrayCounter=0, auxCounter, totalTurnaround=0, cpuUsageMs=0;
    char str[1000], strAux[100];
    processQueue *auxQueue = queue;
    
    sprintf(str, "%u|", auxQueue->submission);
    copyToFile(fileName, str);

    currTime = auxQueue->submission;

    while(auxQueue){
        if(currTime >= auxQueue->submission){ /* Caso o tempo corrente seja maior ou igual a submissão, ele segue normalmente */
            sprintf(str, "P%u %u|", auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
            copyToFile(fileName, str);
        }
        else{ /* Caso o tempo corrente seja menor que o tempo de submissão, haverá um tempo ocioso no cpu */
            currTime = auxQueue->submission;

            sprintf(str, "*** %u|P%u %u|", currTime, auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
            copyToFile(fileName, str);
        }

        cpuUsageMs += auxQueue->cpuBursts[auxQueue->cpuBurstCounter];
        currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
        if(auxQueue->next)
            auxQueue->next->waitTime += currTime;
        auxQueue->endTime = currTime;
        
        /* Array para guardar as E/S dos processos, caso possuam */
        if(auxQueue->quantIoBurst > 0)
            ioBurstEnds[quantIOsArray++] = currTime + auxQueue->ioBursts[auxQueue->ioBurstCounter];
        else{
            finishedProcesses++;
            auxQueue->turnAround = currTime - auxQueue->submission;
        }

        auxQueue = auxQueue->next;
    }

    while(finishedProcesses < quantProcesses){
        sortArray(ioBurstEnds, quantIOsArray);

        auxQueue = queue;
        auxCounter = 0;

        while(ioArrayCounter < quantIOsArray){

            /* Acha o processo que encerrará primeiro */
            while(auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter] != ioBurstEnds[ioArrayCounter])
                auxQueue = auxQueue->next;

            if(auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter] <= currTime){
                sprintf(str, "P%u %u|", auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
                copyToFile(fileName, str);
            }
            else{
                currTime = auxQueue->endTime + auxQueue->ioBursts[auxQueue->ioBurstCounter];

                sprintf(str, "*** %u|P%u %u|", currTime, auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
                copyToFile(fileName, str);
            }
            
            auxQueue->waitTime += currTime - auxQueue->endTime;
            cpuUsageMs += auxQueue->cpuBursts[auxQueue->cpuBurstCounter];

            auxQueue->ioBurstCounter++;
            currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
            auxQueue->endTime = currTime;
            
            /* Caso o processo ainda possua E/S, continua executando */
            if(auxQueue->ioBurstCounter < auxQueue->quantIoBurst){
                ioBurstEndsAux[auxCounter] = currTime + auxQueue->ioBursts[auxQueue->ioBurstCounter];
                auxCounter++;
            }
            else{
                finishedProcesses++;
                auxQueue->turnAround = auxQueue->endTime - auxQueue->submission;
            }

            ioArrayCounter++;  
            auxQueue = queue;
        }

        ioArrayCounter = 0;
        quantIOsArray = auxCounter;

        for(unsigned int i=0;i<auxCounter;i++)
            ioBurstEnds[i] = ioBurstEndsAux[i];
    }

    /* Aqui é calculado o uso de CPU */
    totalTurnaround = currTime - queue->submission;
    float x = (cpuUsageMs * 100) / (totalTurnaround * 1.0);

    sprintf(str, "\nUtilização de CPU: %.2f%%", x);
    copyToFile(fileName, str);

    /* Aqui é calculado o throughput */
    float throughput = quantProcesses * 1.0 / (currTime - queue->submission);

    sprintf(str, "\nThroughput: %.4f processos/segundo", throughput);
    copyToFile(fileName, str);

    /* Aqui é dado o tempo de espera de cada processo */
    unsigned int sum = 0;
    auxQueue = queue;
    memset(str, 0, strlen(str));
    strcat(str, "\nTempo de espera de cada processo: ");
    while(auxQueue){
        sprintf(strAux, "P%u: %ums ", auxQueue->processValue, auxQueue->waitTime);
        strcat(str, strAux);

        sum += auxQueue->waitTime;
        auxQueue = auxQueue->next;
    }

    copyToFile(fileName, str);

    x = sum * 1.0 / quantProcesses;
    sprintf(str, "\nTempo de espera médio: %.1fms", x);
    copyToFile(fileName, str);

    sum = 0;

    auxQueue = queue;
    memset(str, 0, strlen(str));
    strcat(str, "\nTempo de turnaround de cada processo: ");
    while(auxQueue){
        sprintf(strAux, "P%u: %ums ", auxQueue->processValue, auxQueue->turnAround);
        strcat(str, strAux);

        sum += auxQueue->turnAround;
        auxQueue = auxQueue->next;
    }

    copyToFile(fileName, str);

    x = sum * 1.0 / quantProcesses;
    sprintf(str, "\nTempo de turnaround médio: %.1fms", x);

    copyToFile(fileName, str);
}

void SJFAlgorithm(processQueue *queue, unsigned int seq, unsigned int quantProcesses, char *fileName){
    unsigned int currTime, cpuBurstsSorted[quantProcesses], cpuBurstsSortedAux[quantProcesses], burstCounter=0;
    unsigned int finishedProcesses=0, auxCounter=0, burstCounterAux=0;
    char str[1000], numToChar[10];
    processQueue *auxQueue = queue;

    /* Faz a submissão do primeiro processo */
    sprintf(str, "%u|P%u %u|", auxQueue->submission, auxQueue->processValue, auxQueue->submission + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
    copyToFile(fileName, str);

    currTime = auxQueue->submission + auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
    if(auxQueue->cpuBurstCounter < auxQueue->quantCpuBurst)
        cpuBurstsSortedAux[burstCounterAux++] = auxQueue->cpuBursts[auxQueue->cpuBurstCounter];
    auxQueue->endTime = currTime;
    auxQueue = auxQueue->next;

    /* Guarda no array os bursts para comparar qual o menor */
    while(auxQueue){
        cpuBurstsSorted[burstCounter++] = auxQueue->cpuBursts[auxQueue->cpuBurstCounter];
        auxQueue = auxQueue->next;
    }
    auxQueue = queue;

    sortArray(cpuBurstsSorted, burstCounter);

    /* Faz a submissão dos processos na ordem*/
    while(auxCounter < burstCounter){
        while(auxQueue->cpuBursts[auxQueue->cpuBurstCounter] != cpuBurstsSorted[auxCounter])
            auxQueue = auxQueue->next;

        if(auxQueue->submission <= currTime){
            sprintf(str, "P%u %u|", auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
            copyToFile(fileName, str);
        }
        else{
            currTime = auxQueue->submission;

            sprintf(str, "*** %u|P%u %u|", currTime, auxQueue->processValue, currTime + auxQueue->cpuBursts[auxQueue->cpuBurstCounter]);
            copyToFile(fileName, str);
        }

        currTime += auxQueue->cpuBursts[auxQueue->cpuBurstCounter++];
        auxQueue->endTime = currTime;

        if(auxQueue->cpuBurstCounter < auxQueue->quantCpuBurst){
            cpuBurstsSortedAux[burstCounterAux] = auxQueue->cpuBursts[auxQueue->cpuBurstCounter];
            burstCounterAux++;
        }

        auxQueue = queue;
        auxCounter++;
    }

    burstCounter = burstCounterAux;

    for(int i=0;i<burstCounter;i++)
        cpuBurstsSorted[i] = cpuBurstsSortedAux[i];

    for(int i=0;i<burstCounter;i++)
        printf("aaaa %u\n", cpuBurstsSorted[i]);
    
    // while(finishedProcesses < quantProcesses){

    // }

}

void resetStruct(processQueue **queue){
    processQueue *auxQueue = *queue;

    while(auxQueue){    
        auxQueue->cpuBurstCounter = 0;
        auxQueue->endTime = 0;
        auxQueue->ioBurstCounter = 0;
        auxQueue->turnAround = 0;

        auxQueue = auxQueue->next;
    }
}

/* Concatena em determiado arquivo uma string */
void copyToFile(char *fileName, char *insert){
    FILE *file = fopen(fileName, "a+");

    fprintf(file, "%s", insert);

    fclose(file);
}

void sortArray(unsigned int *array, unsigned int n){
    for(int i=1;i<n;i++)
        for(int j=0;j<n - i;j++)
            if(array[j] > array[j+1])
                swap(&array[j], &array[j+1]);
        
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