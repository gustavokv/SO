Faça um programa em C que calcule a quantidade de erros de páginas para os seguintes algoritmos de substituição:

    FIFO;
    OPT;
    LRU.

    Os argumentos de linha de comando do programa são, nessa ordem:
    1) O tamanho em bytes de cada página;
    2) O tamanho em bytes da memória disponível para alocação de páginas;
    3) O nome de um arquivo contendo uma sequência de endereços acessados.
    Esse, no formato texto, conterá endereços separados por um espaço cujos
    valores variam 0 a 65.535. Em cada acesso será referenciado apenas um byte.

Ao final da execução devem ser apresentados na tela, para cada algoritmo, 
o número de erros de página acompanhado do percentual de erros em relação à 
quantidade de endereços acessados. Também deve ser gravado no arquivo texto erros.out
os endereços e páginas que ocasionaram os erros de página para cada algoritmo de substituição.
