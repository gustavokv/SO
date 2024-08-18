//Autor = Gustavo Kermaunar Volobueff
//rgm47006
//volobueffgustavo@gmail.com
//Disciplina de Sistemas Operacionais
//Prof. Dr. Fabrício Sérgio de Paula

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_ARGS 1045
#define MAX_ARG_SIZE 200
#define MAXBUF (BUFSIZ*2)
#define _GNU_SOURCE

struct TreeNode {
    pid_t pid;
	pid_t ppid;
	char name[200];
	struct TreeNode *children[200]; //Guarda os filhos do processo
	struct TreeNode *next; /*Com esta variável criaremos uma lista encadeada que após todos os processos filhos terem sido adicionados
                             à lista encadeada, transformaremos em uma árvore n-ária.*/
};

struct TreeNode *processTree;

//Insere um novo nó na lista encadeada dentro da struct da árvore.
void insertNode(char *proc_name, pid_t pid, pid_t ppid){
    struct TreeNode *node, *aux = processTree;

    node = (struct TreeNode*)malloc(sizeof(struct TreeNode));

    if(!node){
        printf("Erro em alocar memoria para o no.\n");
        return;
    }

    strcpy(node->name, proc_name);

    node->pid = pid;
    node->ppid = ppid;
    node->children[0] = NULL;
    node->next = NULL;
    
    if(!processTree){
        processTree = node;
        return;
    }

    while(aux->next)
        aux = aux->next;

    aux->next = node;
}

//Retorna o PPID de um processo
pid_t getPPid(char *dir){
    char buf[MAXBUF];
    FILE *fp = fopen(dir, "r");
    pid_t ppid;
    unsigned short i=0;
    char *token;

    if(!fp)
        return -1;

    //Abre o stat para pegar o ppid
    size_t ret = fread(buf, sizeof(char), MAXBUF-1, fp);
    buf[ret++] = '\0';

    fclose(fp);

    //Conta até o quarto token para pegar o quarto argumento (ppid)
    token = strtok(buf, " ");
    while(i<3){
        token = strtok(NULL, " ");
        i++;
    }

    ppid = atoi(token);
    return ppid;
}

//Retorna o nome de um PID
char* getProcName(char *dir, char *pid){
    char buf[MAXBUF];
    FILE *fp = fopen(dir, "r");
    unsigned short i=0;
    char *token;

    //Abre o stat para pegar o ppid
    size_t ret = fread(buf, sizeof(char), MAXBUF-1, fp);
    buf[ret++] = '\0';

    fclose(fp);

    //Conta até o quarto token para pegar o quarto argumento (ppid)
    token = strtok(buf, " ");
    while(i<1){
        token = strtok(NULL, " ");
        i++;
    }

    return token;
}

//Acha um nó na lista encadeada para construir a árvore
struct TreeNode* findNode(pid_t pid){
    struct TreeNode *node;
    
	for (node = processTree; node != NULL; node = node->next) {
		if (node->pid == pid)
			return node;
	}
	return NULL;
}

//Comando tree PID
void treeProcess(char *pid_path, unsigned short fatherInsert){
    char proc_path[MAX_ARG_SIZE], proc_name[MAX_ARG_SIZE];
    DIR *proc_dir;
    struct dirent *entity;
    pid_t pid = atoi(pid_path), ppid;   
    unsigned cont_dir_task=0;
    
    //Verificação se o processo existe
    strcpy(proc_path, "/proc/");
    strcat(proc_path, pid_path);

    proc_dir = opendir(proc_path);    
    if(!proc_dir){
        printf("PID nao encontrado!\n");
        return;
    }

    //Insere na lista encadeada o primeiro nó (pai)
    if(!fatherInsert){
        strcat(proc_path, "/stat");
        strcpy(proc_name, getProcName(proc_path, pid_path));
        insertNode(proc_name, pid, 0);
    }

    strcpy(proc_path, "/proc/");
    proc_dir = opendir(proc_path);
    
    //Varre toda o diretório de processos
    while(entity = readdir(proc_dir)){
        if(atoi(entity->d_name) > pid){ //Como todo processo filho tem um valor de PID maior que o PPID, ele só entra caso isso ocorrer
            strcat(proc_path, entity->d_name);
            strcat(proc_path, "/stat");
    
            ppid = getPPid(proc_path);
            if(ppid != -1){ //Há casos em que retornará -1 pois o processo não foi encontrado.
                strcpy(proc_name, getProcName(proc_path, entity->d_name));
                
                if(ppid == pid && !findNode(atoi(entity->d_name))){
                    insertNode(proc_name, atoi(entity->d_name), ppid);
                    treeProcess(entity->d_name, 1); //Depois de inserir um filho, chama novamente o treeProcess para verificar se ele possui filhos
                }
            }

            memset(proc_path, '\0', sizeof(proc_path));
            strcpy(proc_path, "/proc/");
        }
    }

    closedir(proc_dir);
}   

//Com base na lista encadeada criada constrói a árvore
void buildTree(){
	int i;
	struct TreeNode *node, *parent_node;

	for(node = processTree; node != NULL; node = node->next){
		i = 0;
		parent_node = findNode(node->ppid); 

		if (parent_node != NULL) {
			while (parent_node->children[i++] != NULL);

			parent_node->children[i - 1] = node;
			parent_node->children[i] = NULL;
		}
	}
}

//Imprime a árvore de processos
void seeTree(struct TreeNode *tree, int level){
	int i;
	struct TreeNode *node;
    
    for (i = 0; i < level; i++) 
        printf("              ");

	printf("%s(%d)\n", tree->name, tree->pid);
    
	//Recursão nos filhos
	int j = 0;
	while((node = tree->children[j++]) != NULL)
		seeTree(node, level + 1);
}

void clearTree(){
    struct TreeNode *temp = processTree;
    struct TreeNode *aux;

    while(temp != NULL){
        aux = temp->next;
        free(temp);
        temp = aux;
    }

    processTree = NULL;
}

int main(){
    char command[MAX_ARGS];

    while(printf("user@linux ~$ ") && fgets(command, sizeof(command), stdin)){
        command[strlen(command)-1] = '\0'; //Por conta do '\n' do fgets

        //Caso aperte somente enter ele continua no terminal
        if(strcmp(command, "\0")==0)
            continue;

        unsigned short waitProcess=0;

        if(command[strlen(command)-1] == '&'){
            waitProcess++;
            command[strlen(command)-1] = '\0';
        }

        unsigned nArgs=0;
        int status;
        char *token = strtok(command, " ");
        char *args[MAX_ARG_SIZE] = {(char *)0};
        char cmd[MAX_ARG_SIZE];
        pid_t pid;

        if(strcmp(command, "exit") == 0)
            return 0;

        strcpy(cmd, "/bin/");
        strcat(cmd, token);

        //Passa os argumentos para uma matriz por conta do comando execve
        while(token){
            args[nArgs] = malloc(sizeof(MAX_ARG_SIZE));
            strcpy(args[nArgs++], token);

            token = strtok(NULL, " ");
        }

        if(strcmp(args[0], "cd") == 0) //Comando cd pois o execve não o identifica
            chdir(args[1]);
        else if(strcmp(args[0], "tree") == 0){
            treeProcess(args[1], 0);
            buildTree();
            struct TreeNode *auxNode = processTree;
            seeTree(auxNode, 0);
            clearTree();
        }
        else{
            pid = fork();

            if(pid<0)
                fprintf(stderr, "Fork Failed.\n");
            else if(pid == 0){ //Child
                if(execve(cmd, args, __environ) == -1){
                    strcpy(cmd, "/snap/bin/");
                    strcat(cmd, args[0]);
                }

                if(execve(cmd, args, __environ) == -1)
                    printf("Comando nao encontrado!\n");

                exit(1);
            }
            else{ //Parent
                if(!waitProcess) //Para liberar o terminal antes do processo finalizar
                    waitpid(pid, &status, 0);
            }
        }

        //Reseta o array
        for(int i=0;i<nArgs;i++)
            free(args[i]);  
        nArgs=0;
    }

    return 0;
}
