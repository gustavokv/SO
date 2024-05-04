#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>


int sum; /* esses dados são compartilhados pelo(s) thread(s) */
void *runner(void *param); /* os threads chamam essa função */

int main(int argc, char *argv[])
{

	pthread_attr_t attr; /* conjunto de atributos do thread */

	if (argc != 2) {
		fprintf(stderr, "usage: a.out <integer value>\n");
		return -1;
	}
	
	if (atoi(argv[1]) < 0) {
		fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
		return -1;
	}

    int n = atoi(argv[1]);

    pthread_t tid[n]; /* o identificador do thread */


    int indices[n]; //vetor

    for (int i = 1; i <= n; i++) {
        indices[i] = i;
    }

    /* obtém os atributos default */
	pthread_attr_init(&attr);


	/* cria o thread */
    for (int i = 1; i <= n; i++) {
        pthread_create(&tid[n], &attr, runner, &indices[i]);
    }
    
    for (int i = 1; i <= n; i++) {
        /* espera o thread ser encerrado */
	    pthread_join(tid[n], NULL);
    }

	printf("sum = %d\n", sum);
}

/* O thread assumirá o controle nessa função */
void *runner(void *param)
{
	int i, upper = atoi(param);

    int indice = *((int*)param);
	
	sum = 0;
	
	for (i = 1; i <= indice; i++)
		sum += i;

    printf ("INDICE: %d\n" , indice);
	pthread_exit(0);
}