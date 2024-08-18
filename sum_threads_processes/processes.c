#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>

int numIntegers, *integers, *arraySums, *sum=0;

void readFile(char*);
void *runner(void *);

/* Guarda, para cada processo, o inicio e o fim dos subarrays do integers para somar */
typedef struct{
	int start, end, *tempSum;
}ProcessesSubArrays;

int main(int argc, char *argv[]){
    if (argc != 3) {
		fprintf(stderr, "usage: ./a.out <archive name> <n processes>\n");
		return -1;
	}

	if (atoi(argv[2]) < 0) {
		fprintf(stderr, "Number of processes must be >= 0.\n");
		return -1;
	}

    /* Salva o nome do arquivo */
	char *archiveName;
	archiveName = malloc(sizeof(argv[1]));
	strcpy(archiveName, argv[1]);
    readFile(archiveName);

    int nProcesses = atoi(argv[2]);

    if(nProcesses > numIntegers)
        nProcesses = numIntegers;

	int shm_fd = shm_open("/shmfile", O_CREAT | O_RDWR, 0666); /* Cria um objeto para compartilhar memória */
	ftruncate(shm_fd, sizeof(int)); /* Trunca o arquivo criado para o tamanho de um inteiro*/
	sum = (int*)mmap(0, sizeof(int), PROT_WRITE, MAP_SHARED, shm_fd, 0); /* Mapeia o arquivo na memória */

    /* Aceita caso o número de processos seja 0, onde o processo principal faz a soma*/
	if(nProcesses == 0){
		for(int i=0;i<numIntegers;i++)
			*sum += integers[i];

		printf("Sum = %d\n", *sum);

		shm_unlink("/shmfile");

		free(archiveName);
		free(integers);
		return 0;
	}

	pid_t pid[nProcesses];
	ProcessesSubArrays subArraysIndices[nProcesses];
	arraySums = malloc(sizeof(int) * nProcesses);

	for(int i=0;i<nProcesses;i++)
		arraySums[i] = 0;

	/* Passo para cada subArray */
	int quantPerSubArray = (int)(numIntegers / nProcesses);

	/* Calcula as posições dos subarrays para os processos somá-las */
	subArraysIndices[0].start = 0;
	subArraysIndices[0].end = quantPerSubArray-1;
	for(int i=1;i<nProcesses;i++){
		subArraysIndices[i].start = subArraysIndices[i-1].end+1;

		if(subArraysIndices[i].start + quantPerSubArray-1 >= numIntegers)
			subArraysIndices[i].end = numIntegers-1;
		else
			subArraysIndices[i].end = subArraysIndices[i].start + quantPerSubArray-1;
	}
	if(subArraysIndices[nProcesses-1].end < numIntegers)
		subArraysIndices[nProcesses-1].end = numIntegers-1;

	for(int i=0;i<nProcesses;i++){
		pid[i] = fork();

		if(pid[i] < 0){
			fprintf(stderr, "Process in indice position [%d] has failed.\n", i);
			return -1;
		}
		else if(pid[i] == 0){ /* Child */
			subArraysIndices[i].tempSum = &arraySums[i];
			runner(&subArraysIndices[i]);
			*sum += arraySums[i];

			exit(0);
		}
	}
	
	/* Espera os processos encerrarem */
	for(int i=0;i<nProcesses;i++){
		waitpid(pid[i], NULL, 0);
	}
	
	printf("Sum = %d\n", *sum);

	shm_unlink("/shmfile");

    free(archiveName);
    free(integers);
	free(arraySums);

    return 0;
}

/* Os processos assumirão o controle nessa função */
void *runner(void *param){
	ProcessesSubArrays *process = (ProcessesSubArrays*)param;
	int tempSum=0;

	for (int i = process->start; i <= process->end; i++){
		tempSum += integers[i];
	}

	*(process)->tempSum = tempSum;
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