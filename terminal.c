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

static struct TreeNode *head;

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
    node->next = head;
}

pid_t checkPPid(char *dir){
    char buf[MAXBUF];
    FILE *p_file = fopen(dir, "r");
    pid_t ppid;
    unsigned short i=0;
    char *token;

    size_t ret = fread(buf, sizeof(char), MAXBUF-1, p_file);
    buf[ret++] = '\0';

    fclose(p_file);

    token = strtok(buf, " ");

    printf("%s\n", dir);
    while(i<4){
        printf("%s\n", token);
        token = strtok(NULL, " ");
        i++;
    }
    printf("%s\n", token);
    ppid = atoi(token);

    if(ppid == 0)
        exit(0);

    return ppid;
}

//Comando tree PID
void treeProcess(char *pid_path){
    char *proc_path;
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

    strcat(proc_path, "/task/");

    proc_dir = opendir(proc_path);

    while(entity = readdir(proc_dir))
        cont_dir_task++;

    if(cont_dir_task > 3){ //Caso os nomes dos diretórios sejam os filhos
        
    }
    else{ //Caso esteja no arquivo children
        strcat(proc_path, pid_path);
        
        

    }

    closedir(proc_dir);

    // strcpy(proc_path, "/proc/");
    // proc_dir = opendir(proc_path);

    
    // while(entity = readdir(proc_dir)){
    //     if(atoi(entity->d_name) > pid){
    //         strcat(proc_path, entity->d_name);
    //         strcat(proc_path, "/stat");

    //         ppid = checkPPid(proc_path);

    //         memset(proc_path, '\0', sizeof(proc_path));
    //         strcpy(proc_path, "/proc/");
    //     }
    // }
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
        else if(strcmp(args[0], "tree") == 0)
            treeProcess(args[1]);
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
