#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp);
void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp);
void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp);
int search_array(unsigned int *arr, unsigned int x, unsigned int arr_size); 

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

    fp = fopen("erros.out", "w");

    fwrite("FIFO: ", 1, sizeof("FIFO: "), fp);
    FIFO(access_addr, quant_elem, &fp);
    
    fwrite("\n\nOPT: ", 1, sizeof("\n\nOPT: "), fp);
    OPT(access_addr, quant_elem, &fp);  

    fwrite("\n\nLRU: ", 1, sizeof("\n\nLRU: "), fp);
    LRU(access_addr, quant_elem, &fp);

    fclose(fp);

    return 0;
}

int search_array(unsigned int *arr, unsigned int x, unsigned int arr_size){
    for(int i=0;i<arr_size;i++)
        if(arr[i] == x)
            return i;

    return -1;
}

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp){
    
}

void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp){

}

void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp){

}