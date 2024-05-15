#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int *arraySums, sum=0; 
int numIntegers=0, indexSum=0, *integers;

void *runner(void *param);
void readFile(char*);

/* Guarda, para cada thread, o inicio e o fim do array integers para somar */
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
	arraySums = malloc(sizeof(int) * nThreads);

	for(int i=0;i<nThreads;i++)
		arraySums[i] = 0;

	readFile(archiveName);

	if(nThreads == 0){
		for(int i=0;i<numIntegers;i++)
			sum += integers[i];

		printf("Sum = %d\n", sum);

		free(archiveName);
		free(integers);
		free(arraySums);
		return 0;
	}

	int quantPerSubArray = (int)(numIntegers / nThreads);
	
	/* Calcula as posições dos subarrays para as threads somá-las */
	subArraysIndices[0].start = 0;
	subArraysIndices[0].end = quantPerSubArray-1;
	for(int i=1;i<nThreads;i++){
		subArraysIndices[i].start = subArraysIndices[i-1].end+1;

		if(subArraysIndices[i].start + quantPerSubArray-1 >= numIntegers)
			subArraysIndices[i].end = numIntegers-1;
		else
			subArraysIndices[i].end = subArraysIndices[i].start + quantPerSubArray-1;
	}
	if(subArraysIndices[nThreads-1].end < numIntegers)
		subArraysIndices[nThreads-1].end = numIntegers-1;

	pthread_attr_t attr;
	pthread_attr_init(&attr);

	/* Cria as threads */
    for (int i = 0; i < nThreads; i++){
    	pthread_create(&tid[i], &attr, runner, &subArraysIndices[i]);
	}
    
	/* Espera o thread ser encerrado */
    for (int i = 0; i < nThreads; i++){
	    pthread_join(tid[i], NULL);
	}

	/* Soma os resultados dos subarrays */
	for(int i=0;i<nThreads;i++)
		sum += arraySums[i];

	printf("Sum = %d\n", sum);

	free(archiveName);
	free(integers);
	free(arraySums);
	return 0;
}

/* As threads assumirão o controle nessa função */
void *runner(void *param){
	ThreadsSubArrays *threads = (ThreadsSubArrays*)param;

	for (int i = threads->start; i <= threads->end; i++){
		arraySums[indexSum] += integers[i];
	}
	indexSum++;

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