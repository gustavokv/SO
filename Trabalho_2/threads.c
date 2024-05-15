#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long int sum;
int *integers, numIntegers=0;

void *runner(void *param);
void readFile(char*);

typedef struct{
	int start, end;
}ThreadsSubArrays;

int main(int argc, char *argv[]){
	if (argc != 3) {
		fprintf(stderr, "usage: ./a.out <archive name> <n threads>\n");
		return -1;
	}

	if (atoi(argv[2]) < 0) {
		fprintf(stderr, "Number of threads must be >= 0.\n");
		return -1;
	}

	char *archiveName;
	archiveName = malloc(sizeof(argv[1]));
	strcpy(archiveName, argv[1]);

	int nThreads = atoi(argv[2]);
	pthread_t tid[nThreads];
	ThreadsSubArrays subArraysIndices[nThreads];

	readFile(archiveName);

	if(nThreads == 0){
		for(int i=0;i<numIntegers;i++)
			sum += integers[i];

		printf("Sum = %ld.\n", sum);

		free(archiveName);
		free(integers);
		return 0;
	}

	int quantPerSubArray = ceil(numIntegers / nThreads);

	subArraysIndices[0].start = 0;
	subArraysIndices[0].end = quantPerSubArray-1;
	for(int i=1;i<nThreads;i++){
		subArraysIndices[i].start = subArraysIndices[i-1].start+1;
		subArraysIndices[i].end = subArraysIndices[i].start * quantPerSubArray;
	}

	// /* Cria as threads */
    // for (int i = 0; i < nThreads; i++)
    // 	pthread_create(&tid[nThreads], NULL, &runner, &integers[i]);
    
	// /* Espera o thread ser encerrado */
    // for (int i = 0; i < nThreads; i++)
	//     pthread_join(tid[nThreads], NULL);

	free(archiveName);
	free(integers);
	return 0;
}

/* O thread assumirá o controle nessa função */
void *runner(void *param){
	int i, upper = atoi(param);
    int indice = *((int*)param);
	
	sum = 0;
	
	for (i = 1; i <= indice; i++)
		sum += i;

	pthread_exit(0);
}

/* Lê o arquivo e guarda no array de inteiros */
void readFile(char *archiveName){
	FILE *fp;
	int i=0, a;

	fp = fopen(archiveName, "rb");

	if(!fp){
		printf("Archive <%s> wasn't found.\n", archiveName);
		exit(0);
	}

	/* Calcula a quantidade de inteiros do array no arquivo */
	fseek(fp, 0, SEEK_END);
	numIntegers = ftell(fp) / sizeof(int);
	rewind(fp);
	
	integers = malloc(numIntegers * sizeof(int));
	while(fread(&integers[i++], sizeof(int), 1, fp)); /* Armazena no array integers os inteiros do arquivo */
	
	fclose(fp);
}