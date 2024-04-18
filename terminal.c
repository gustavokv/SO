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

#define MAX_ARGS 1045
#define MAX_ARG_SIZE 40
#define MAXBUF (BUFSIZ*2)
#define _GNU_SOURCE

struct TreeNode {
	char name[128];
	pid_t pid;
	pid_t ppid;
	struct TreeNode *parent;
	struct TreeNode *children[128];
	struct TreeNode *next;
};

static struct TreeNode *processTree;

//Insere um novo nó para a árvore de processos.
void insertNode(char *proc_name, pid_t pid, pid_t ppid){
    struct TreeNode *node;

    node = (struct TreeNode*)malloc(sizeof(struct TreeNode));
    if(!node){
        printf("Erro em alocar memoria para o no.\n");
        return;
    }

    strcpy(node->name, proc_name);
    node->pid = pid;
    node->ppid = ppid;
    node->children[0] = NULL;
    node->parent = NULL;
    node->next = processTree;
    processTree = node;
}

pid_t getPPid(char *dir){
    char buf[MAXBUF];
    FILE *p_file = fopen(dir, "r");
    pid_t ppid;
    unsigned short i=0;
    char *token;

    //Abre o stat para pegar o ppid
    size_t ret = fread(buf, sizeof(char), MAXBUF-1, p_file);
    buf[ret++] = '\0';

    fclose(p_file);

    //Conta até o quarto token para pegar o quarto argumento (ppid)
    token = strtok(buf, " ");
    while(i<3){
        token = strtok(NULL, " ");
        i++;
    }

    ppid = atoi(token);
    return ppid;
}

char* getProcName(char *dir, char *pid){
    char buf[MAXBUF];
    FILE *p_file = fopen(dir, "r");
    unsigned short i=0;
    char *token;

    //Abre o stat para pegar o ppid
    size_t ret = fread(buf, sizeof(char), MAXBUF-1, p_file);
    buf[ret++] = '\0';

    fclose(p_file);

    //Conta até o quarto token para pegar o quarto argumento (ppid)
    token = strtok(buf, " ");
    while(i<1){
        token = strtok(NULL, " ");
        i++;
    }

    return token;
}

//Acha um nó na lista encadeada formada para construir a árvore
struct TreeNode* findNode(pid_t pid){
    struct TreeNode *node;
    
	for (node = processTree; node != NULL; node = node->next) {
		if (node->pid == pid)
			return node;
	}
	return NULL;
}

//Comando tree PID
void treeProcess(char *pid_path){
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

    strcat(proc_path, "/stat");
    strcpy(proc_name, getProcName(proc_path, pid_path));
    insertNode(proc_name, pid, 0);
    
    strcpy(proc_path, "/proc/");
    proc_dir = opendir(proc_path);
    
    while(entity = readdir(proc_dir)){
        if(atoi(entity->d_name) > pid){
            strcat(proc_path, entity->d_name);
            strcat(proc_path, "/stat");
            
            ppid = getPPid(proc_path);
            strcpy(proc_name, getProcName(proc_path, entity->d_name));

            if(ppid == pid && !findNode(ppid)){
                insertNode(proc_name, atoi(entity->d_name), ppid);
                treeProcess(entity->d_name);
            }

            memset(proc_path, '\0', sizeof(proc_path));
            strcpy(proc_path, "/proc/");
        }
    }

    closedir(proc_dir);
}   

void buildTree(){
	int i;
	struct TreeNode *node, *pnode;

	/* now we have a valid linked list, make a tree */
	for(node = processTree; node != NULL; node = node->next) {
		i = 0;

		pnode = findNode(node->ppid);

		if(pnode != NULL) {
			node->parent = pnode;

			while (pnode->children[i++] != NULL);

			pnode->children[i - 1] = node;
			pnode->children[i] = NULL;
		}
	}
}

//Imprime a árvore de processos
void seeTree(struct TreeNode * root, int level){
	int i;
	struct TreeNode *node;

	for (i = 0; i < level; i++) 
		printf("  ");

	printf("%s pid: %d, ppid: %d\n", root->name, root->pid, root->ppid);

	//Recursão nos filhos
	int j = 0;
	while((node = root->children[j++]) != NULL)
		seeTree(node, level + 1);
}

//Limpa os ponteiros para a próxima árvore
void clearTree(){

}

int main(){
    char command[MAX_ARGS];

    while(printf("user@linux ~$ ") && fgets(command, sizeof(command), stdin)){
        command[strlen(command)-1] = '\0';

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
            treeProcess(args[1]);
            buildTree();

            struct TreeNode *node;
            for (node = processTree; node != NULL; node = node->next) {
                if (node->parent == NULL)
                    seeTree(node, 0);
            }

            clearTree();
        }
        else{
            pid = fork();

            if(pid<0)
                fprintf(stderr, "Fork Failed.\n");
            else if(pid == 0){ //Child
                execve(cmd, args, __environ);
                exit(1);
            }
            else{ //Parent
                if(!waitProcess) //Para liberar o terminal antes do processo finalizar
                    waitpid(pid, &status, 0);
            }
        }

        //Reseta o array por conta do malloc
        for(int i=0;i<nArgs;i++)
            free(args[i]);  

        nArgs=0;
    }

    return 0;
}
