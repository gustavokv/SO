Faça um programa multithread que funcione da seguinte forma:

    Inicialmente, a thread principal cria 3 trheads adicionais;
    
    Em seguida, a thread principal realiza a leitura de números naturais 
    do arquivo texto in.txt, armazenando-os no final da lista simplesmente encadeada L;
    
    Uma trhead adicional analisa os números armazenados em L e remove dessa lista os 
    elementos pares maiores que 2;
    
    Outra thread adicional analisa os números armazenados em L e remove os não primos;
    Outra thread adicional imprime os números primos armazenados em L.

Para a sincronização, utilize apenas semáforos binários 
(veja página pthreads do manual do Linux). Projete uma solução que permita
todas as threads progredirem simultaneamente. Cada nó da lista deve conter seu 
próprio semáforo e pode armazenar outras informações para fins de sincronização,
caso necessário (ex.: contador do passo executado no nó). 
