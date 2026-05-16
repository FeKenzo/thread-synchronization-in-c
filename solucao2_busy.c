/*
 * SOLUÇÃO 2 — Variável busy (Espera Ocupada / Busy Waiting)
 *
 * Utiliza uma variável global "busy" como flag de controle de acesso.
 * Enquanto busy == 1, a thread fica em loop aguardando (espera ocupada).
 * Quando busy == 0, a thread entra na região crítica e seta busy = 1.
 *
 * PROBLEMAS DESTA SOLUÇÃO:
 *
 * 1) Ainda há condição de disputa!
 *    O teste (busy == 0) e a atribuição (busy = 1) são operações separadas
 *    e não atômicas. Entre o teste e a atribuição, o SO pode trocar o
 *    contexto e outra thread pode entrar na região crítica simultaneamente.
 *    Exemplo de intercalação problemática:
 *      T1: lê busy → 0  (vai entrar)
 *      [troca de contexto]
 *      T2: lê busy → 0  (também vai entrar!)
 *      T2: seta busy = 1, entra na RC
 *      T1: seta busy = 1, entra na RC  ← DUAS THREADS NA RC AO MESMO TEMPO!
 *
 * 2) Desperdício de CPU (busy waiting):
 *    A thread que aguarda fica em loop contínuo consumindo 100% do núcleo
 *    de CPU enquanto espera, sem realizar trabalho útil. Em sistemas reais,
 *    isso reduz a eficiência e pode causar starvation.
 *
 * Compile: gcc solucao2_busy.c -o solucao2_busy -lpthread
 */

#include <stdio.h>
#include <pthread.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int saldo = 0;
int busy   = 0; /* 0 = livre, 1 = ocupado */

void* deposito(void* arg)
{
    for (int i = 0; i < REPETICOES; i++)
    {
        /* Espera ocupada: consome CPU enquanto a RC está ocupada */
        while (busy == 1)
            ; /* busy waiting — problema de eficiência */

        busy = 1;           /* marca como ocupado — NÃO é atômico com o teste! */

        saldo = saldo + 1;  /* região crítica */

        busy = 0;           /* libera a RC */
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, deposito, NULL);
    pthread_create(&t2, NULL, deposito, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Saldo final   : %d\n", saldo);
    printf("Valor esperado: %d\n", ESPERADO);
    printf("Resultado correto: %s\n", saldo == ESPERADO ? "SIM" : "NAO");
    printf("NOTA: execute varias vezes — o resultado pode variar!\n");

    return 0;
}
