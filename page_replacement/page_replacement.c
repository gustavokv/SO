#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte);
void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte);
void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte);
int search_array(int *arr, unsigned int x, unsigned int arr_size, unsigned int init); 
void clearPages(int *pages, unsigned int pages_size);

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
    unsigned int pages_size = (int)(mem_size/page_size*1.0);
    int pages[pages_size];

    //Inicialização do array com -1 pois pode ocorrer comparações com valores aleatórios.
    for(int i=0;i<pages_size;i++)
        pages[i] = -1;

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
    FIFO(access_addr, quant_elem, fp, pages, pages_size, page_size);

    clearPages(pages, pages_size);
    
    fprintf(fp, "\n\n%s", "OPT:     ENDERECO     PAGINA");
    OPT(access_addr, quant_elem, fp, pages, pages_size, page_size); 

    clearPages(pages, pages_size); 

    fprintf(fp, "\n\n%s", "LRU:     ENDERECO     PAGINA");
    LRU(access_addr, quant_elem, fp, pages, pages_size, page_size);

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

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte){
    unsigned int quant_errors=0, page_pos=0;

    for(int i=0;i<quant_elem;i++){
        if(search_array(pages, access_addr[i], pages_size, 0) < 0 ){
            fprintf(fp, "\n              %u           %u", access_addr[i], i*each_page_byte);
            quant_errors++; 

            if(page_pos > pages_size-1)
                page_pos = 0;

            pages[page_pos++] = access_addr[i];
        }
    }

    printf("FIFO:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte){
    unsigned int quant_errors=0, page_pos=0, quant_elem_page=0;
    int maior, verif=-1, init;

    for(int i=0;i<quant_elem;i++){
        if(search_array(pages, access_addr[i], pages_size, 0) < 0){
            fprintf(fp, "\n              %u           %u", access_addr[i], i*each_page_byte);
            quant_errors++;

            if(quant_elem_page < pages_size){
                pages[page_pos] = access_addr[i];
                page_pos++;
                quant_elem_page++;
            }
            else{
                init = i+1;
                maior = -1;
                
                for(int j=0;j<pages_size;j++){
                    if((verif = search_array(access_addr, pages[j], quant_elem, init)) > maior || verif == -1){
                        maior = verif;
                        page_pos = j;

                        if(verif == -1){
                            maior = 1;
                            break;
                        }
                    }
                }

                if(maior < 0)
                    if(page_pos+1 == pages_size)
                        page_pos = 0;
                    else
                        page_pos++;

                pages[page_pos] = access_addr[i];
            }
        }
    }

    printf("OPT:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *pages, unsigned int pages_size, unsigned int each_page_byte){
    unsigned int quant_errors=0;


    printf("LRU:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));        
}