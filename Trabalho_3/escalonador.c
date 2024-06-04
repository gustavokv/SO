#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct processQueue{
    unsigned int priority;
    unsigned int submission;
    unsigned int *bursts;
    struct processQueue *next;
} processQueue;

void insertFileInQueue(char *archName, processQueue **queue);

int main(int argc, char *argv[]){
    char *archName = argv[1];
    unsigned int quantum = atoi(argv[2]);
    unsigned int sequential = 0;

    processQueue queue;

    if(argc == 4 && strcpy(argv[3], "-seq") == 0)
        sequential++;

    insertFileInQueue(archName, &queue);

    

    return 0;
}

void insertFileInQueue(char *archName, processQueue **queue){
    FILE *fp = fopen(archName, "rb");

    if(!fp){
        printf("File <%s> not found!\n", archName);
        exit(-1);
    }

    fseek(fp, 0L, SEEK_END);
    unsigned int size = ftell(fp);
    rewind(fp);
    
    char *contentsArray = malloc(size);
    int i=0;

    while(fread(&contentsArray[i++], sizeof(char), 1, fp));

    i=0;
    char *token = strtok(contentsArray, " ");
    unsigned int burstCount=0;

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
                if(strcmp(token, "\n") == 0)
                    break;

                newNode->bursts[burstCount++] = atoi(token);
            }

            i++;    
        } while(token = strtok(NULL, " "));

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

    fclose(fp);


}