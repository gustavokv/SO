#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_ARRAY_SIZE 65536

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size);
void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size);
// void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size);
int search_array(int *arr, unsigned int x, unsigned int arr_size, unsigned int init); 
void clearPages(int *pages, unsigned int pages_size);

int main(int argc, char *argv[]){

    if(argc != 4){
        printf("Usage: ./a.out <page size> <memory size> <archive name>\n");
        return -1;
    }

    unsigned int page_size = atoi(argv[1]);
    unsigned int mem_size = atoi(argv[2]);
    char arch_name[sizeof(argv[3]) * sizeof(int)];
    strcpy(arch_name, argv[3]);
    
    FILE *fp;

    fp = fopen(arch_name, "r");

    if(!fp){
        printf("Archive %s not found.\n", arch_name);
        return -1;
    }
    
    int access_addr[MAX_ARRAY_SIZE], quant_elem=0, value, quant_frames = mem_size / page_size;
    int page_table[quant_frames];

    //Inicialização do array com -1 pois pode ocorrer comparações com valores aleatórios.
    for(int i=0;i<quant_frames;i++)
        page_table[i] = -1;
    
    fscanf(fp, "%d", &value);
    while(!feof(fp)){
        access_addr[quant_elem++] = value;
        fscanf(fp, "%d", &value);
    }
    
    fclose(fp);

    fp = fopen("erros.txt", "r");

    if(fp)
        remove("erros.txt");

    fclose(fp);
    fp = fopen("erros.txt", "a+");  
    
    fprintf(fp, "%s", "FIFO:     ENDERECO     PAGINA");
    FIFO(access_addr, quant_elem, fp, page_table, quant_frames, page_size);
    
    clearPages(page_table, quant_frames);
    
    fprintf(fp, "\n\n%s", "OPT:     ENDERECO     PAGINA");
    OPT(access_addr, quant_elem, fp, page_table, quant_frames, page_size); 

    // clearPages(pages, pages_size); 

    // fprintf(fp, "\n\n%s", "LRU:     ENDERECO     PAGINA");
    // LRU(access_addr, quant_elem, fp, pages, pages_size, page_size);

    fclose(fp);

    return 0;
}

void clearPages(int *pages, unsigned int pages_size){
    for(int i=0;i<pages_size;i++)
        pages[i] = -1;
}

int search_array(int *arr, unsigned int x, unsigned int arr_size, unsigned int init){
    for(int i=init;i<arr_size;i++){
        if(arr[i] == x)
            return i;
    }

    return -1;
}

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size){
    unsigned int quant_errors=0, page_pos=0;

    for(int i=0;i<quant_elem;i++){
        if(search_array(page_table, (int)(access_addr[i]/page_size), quant_frames, 0) < 0){
            fprintf(fp, "\n              %u           %u", access_addr[i], (int)(access_addr[i] / page_size));
            
            page_table[page_pos++] = (int)(access_addr[i]/page_size);
            quant_errors++; 

            if(page_pos >= quant_frames)
                page_pos = 0;
        }
    }

    printf("FIFO:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size){
    unsigned int quant_errors=0, page_pos=0;

    for(int i=0;i<quant_elem;i++){
        
    }

    printf("OPT:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte){
    unsigned int quant_errors=0;


    printf("LRU:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));        
}