#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){

    if(argc != 4){
        printf("Usage: ./a.out <page size> <memory size> <archive name>\n");
        return -1;
    }

    unsigned int page_size = atoi(argv[1]);
    unsigned int mem_size = atoi(argv[2]);
    char arch_name[sizeof(argv[3])];
    strcpy(arch_name, argv[3]);

    FILE *fp;
    unsigned int arch_size;
    struct stat st;

    fp = fopen(arch_name, "r");

    if(!fp){
        printf("Archive %s not found.\n", arch_name);
        return -1;
    }

    stat(arch_name, &st);
    arch_size = st.st_size;
    unsigned int access_addr[arch_size], quant_elem=0, value;

    fscanf(fp, "%d", &value);
    while(!feof(fp)){
        access_addr[quant_elem++] = value;
        fscanf(fp, "%d", &value);
    }

    fclose(fp);


    return 0;
}