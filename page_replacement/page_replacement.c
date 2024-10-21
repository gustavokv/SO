/* 
    Aluno: Gustavo Kermaunar Volobueff rgm47006
    Prof. Dr. Fabrício Sérgio de Paula
    Disciplina de Sistemas Operacionais 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size);
void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size);
void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size);

int search_array(int *arr, unsigned int x, unsigned int arr_size, unsigned int init); 
void clearPages(int *pages, unsigned int pages_size);
int search_OPT(unsigned int *arr, unsigned int x, unsigned int quant_elem, unsigned int init, unsigned int page_size);
int search_furthest(int *arr, unsigned int x, unsigned int arr_size, unsigned int init, unsigned int page_size);

int main(int argc, char *argv[]){
    if(argc != 4){
        printf("Usage: ./a.out <page size> <memory size> <archive name>\n");
        return -1;
    }

    unsigned int page_size = atoi(argv[1]);
    unsigned int mem_size = atoi(argv[2]);

    if(mem_size < page_size){
        printf("Memory has to be greater than page size.\n");
        return -1;
    }

    char file_name[sizeof(argv[3]) * sizeof(int)];
    strcpy(file_name, argv[3]);
    
    FILE *fp;
    struct stat st;
    int size;

    fp = fopen(file_name, "r");

    if(!fp){
        printf("Archive %s not found.\n", file_name);
        return -1;
    }

    stat(file_name, &st);
    size = st.st_size;
    
    int access_addr[size*sizeof(int)], quant_elem=0, value, quant_frames = mem_size / page_size;
    int page_table[quant_frames];

    /* Inicialização do array com -1 pois pode ocorrer comparações com valores aleatórios, sendo eles 0 */
    for(int i=0;i<quant_frames;i++)
        page_table[i] = -1;
    
    fscanf(fp, "%d", &value);
    while(!feof(fp)){
        access_addr[quant_elem++] = value;
        fscanf(fp, "%d", &value);
    }
    
    fclose(fp);

    fp = fopen("erros.out", "rb");

    if(fp)
        remove("erros.out");

    fp = fopen("erros.out", "ab"); 
    
    fprintf(fp, "%s", "FIFO:     ENDERECO     PAGINA");
    FIFO(access_addr, quant_elem, fp, page_table, quant_frames, page_size);
    
    clearPages(page_table, quant_frames);
    
    fprintf(fp, "\n\n%s", "OPT:     ENDERECO     PAGINA");
    OPT(access_addr, quant_elem, fp, page_table, quant_frames, page_size); 

    clearPages(page_table, quant_frames); 

    fprintf(fp, "\n\n%s", "LRU:     ENDERECO     PAGINA");
    LRU(access_addr, quant_elem, fp, page_table, quant_frames, page_size);

    fclose(fp);

    return 0;
}

/* Limpa a tabela de páginas para o próximo algoritmo */
void clearPages(int *pages, unsigned int pages_size){
    for(int i=0;i<pages_size;i++)
        pages[i] = -1;
}

int search_array(int *arr, unsigned int x, unsigned int arr_size, unsigned int init){
    for(int i=init;i<arr_size;i++)
        if(arr[i] == x)
            return i;

    return -1;
}

/* Faz a contagem de elementos que se repetem dentro da tabela de páginas nos endereços acessados para o OPT */
int search_OPT(unsigned int *arr, unsigned int x, unsigned int quant_elem, unsigned int init, unsigned int page_size){
    unsigned int count=0;

    for(int i=init;i<quant_elem;i++)
        if((int)(arr[i]/page_size) == x)
            count++;

    if(count>0)
        return count;

    return -1;
}

int search_furthest(int *arr, unsigned int x, unsigned int arr_size, unsigned int init, unsigned int page_size){
    for(int i=init;i<arr_size;i++)
        if((int)(arr[i]/page_size) == x)
            return i;

    return -1;
}

void FIFO(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size){
    unsigned int quant_errors=0, page_pos=0;

    for(int i=0;i<quant_elem;i++){
        if(search_array(page_table, (int)(access_addr[i]/page_size), quant_frames, 0) < 0){
            fprintf(fp, "\n              %u           %u", access_addr[i], (int)(access_addr[i] / page_size));
            
            page_table[page_pos++] = (int)(access_addr[i]/page_size);
            quant_errors++; 

            /* Volta ao início da tabela de páginas */
            if(page_pos >= quant_frames)
                page_pos = 0;
        }
    }

    printf("FIFO:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void OPT(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size){
    unsigned int quant_errors=0, page_pos=0, is_full=0, see_same[quant_frames], same_count;
    int menor, search_res, maior;

    for(int i=0;i<quant_elem;i++){
        if(search_array(page_table, (int)(access_addr[i]/page_size), quant_frames, 0) < 0){
            fprintf(fp, "\n              %u           %u", access_addr[i], (int)(access_addr[i] / page_size));

            menor = quant_elem;
            maior = 0;
            same_count = 0;

            if(is_full<quant_frames) /* Preenche a tabela de páginas */
                page_table[is_full++] = (int)(access_addr[i]/page_size);
            else{
                for(int j=0;j<quant_frames;j++){ /* Para todos os valores da tabela de páginas, conta o com menor frequência dentro do array de accesos */
                    if((search_res = search_OPT(access_addr, page_table[j], quant_elem, i, page_size)) < menor){
                        same_count=0;

                        see_same[same_count++] = page_table[j];
                        menor = search_res; 
                        page_pos = j;
                    }
                    else if(search_res == menor)
                        see_same[same_count++] = page_table[j];
                    
                    /* Caso em que o valor não aparece depois no array de acessos */
                    if(search_res == -1){
                        page_pos = j;
                        break;              
                    }
                }

                /* Caso haja uma quantidade igual de aparições, ele verifica qual a mais "distante" */
                if(same_count>1){
                    for(int j=0;j<same_count;j++){
                        if((search_res = search_furthest(access_addr, see_same[j], quant_elem, i, page_size)) > maior){
                            maior = search_res;
                            page_pos = search_array(page_table, see_same[j], quant_frames, 0);
                        }    
                    }
                }                   

                page_table[page_pos] = (int)(access_addr[i]/page_size);
            }

            quant_errors++;
        }
    }

    printf("OPT:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));
}

void LRU(unsigned int *access_addr, unsigned int quant_elem, FILE *fp, int *page_table, unsigned int quant_frames, unsigned int page_size){
    unsigned int quant_errors=0, n_frames=0, maior_tempo, page_pos;
    int contador[quant_frames], pos;

    //Vamos utilizar um contador para o LRU
    for(int i=0;i<quant_frames;i++)
        contador[i] = 0;

    for(int i=0;i<quant_elem;i++){
        pos = search_array(page_table, (int)(access_addr[i]/page_size), quant_frames, 0);

        if(pos < 0){
            fprintf(fp, "\n              %u           %u", access_addr[i], (int)(access_addr[i] / page_size));

            maior_tempo=0;

            if(n_frames<quant_frames)
                page_table[n_frames++] = (int)(access_addr[i]/page_size);
            else{
                for(int j=0;j<quant_frames;j++) /* Verifica o maior tempo dentro da tabela de páginas */
                    if(contador[j] > maior_tempo){
                        maior_tempo = contador[j];
                        page_pos = j;
                    }
            
                contador[page_pos] = 0; /* Reseta pois é um novo acesso */
                page_table[page_pos] = (int)(access_addr[i]/page_size);
            }

            quant_errors++;
        }
        else
            contador[pos] = 0; /* Caso de acceso, portanto reseta o contador */

        /* Incrementa o contador em cada posição */
        for(int j=0;j<n_frames;j++)
            contador[j]++;
    }

    printf("LRU:     QUANTIDADE DE ERROS     PERCENTUAL DE ERROS\n\
                   %u                   %.2f%%\n", quant_errors, ((100*quant_errors)/(quant_elem*1.0)));        
}