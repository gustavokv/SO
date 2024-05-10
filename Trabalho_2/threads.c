#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sum;
int *integers, numIntegers=0;
char *archiveName;

void *runner(void *param);
void *readFile();

int main(int argc, char *argv[]){
	if (argc != 3) {
		fprintf(stderr, "usage: ./a.out <archive name> <n threads/processes>\n");
		return -1;
	}
	
	if (atoi(argv[1]) < 0) {
		fprintf(stderr, "%d must be >= 0\n", atoi(argv[2]));
		return -1;
	}

	archiveName = malloc(sizeof(argv[1]));
	strcpy(archiveName, argv[1]);

	int nThreads = atoi(argv[2]);
	pthread_t tid[nThreads];

	pthread_create(&tid[0], NULL, readFile, NULL);
	pthread_join(tid[0], NULL);

	/* cria o thread */
    for (int i = 1; i <= nThreads; i++) {
    	pthread_create(&tid[nThreads], NULL, &runner, &indices[i]);
    }
    
    for (int i = 1; i <= nThreads; i++) {
        /* espera o thread ser encerrado */
	    pthread_join(tid[nThreads], NULL);
    }


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

//Thread principal lê o arquivo e guarda no array de inteiros
void *readFile(){
	FILE *fp;
	int i=0, a;

	fp = fopen(archiveName, "rb");

	if(!fp){
		printf("Archive <%s> wasn't found.\n", archiveName);
		exit(0);
	}

	//Calcula a quantidade de inteiros do arquivo
	fseek(fp, 0, SEEK_END);
	numIntegers = ftell(fp) / sizeof(int);
	rewind(fp);
	
	integers = malloc(numIntegers * sizeof(int));
	while(fread(&integers[i++], sizeof(int), 1, fp));

	fclose(fp);
	pthread_exit(0);
}