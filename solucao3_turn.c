/*
 * SOLUÇÃO 3 — Alternância de Uso (variável turn)
 *
 * A variável "turn" indica qual thread tem permissão para entrar na
 * região crítica. As threads se alternam estritamente: T0, T1, T0, T1...
 *
 * GARANTE EXCLUSÃO MÚTUA?
 * Sim. Apenas a thread cujo ID corresponde ao valor de "turn" pode entrar
 * na região crítica. A outra fica em espera ocupada.
 *
 * PROBLEMA — INDEPENDÊNCIA NÃO É GARANTIDA:
 * A alternância é estrita e obrigatória. Se T0 não quiser mais entrar na
 * região crítica, T1 ficará bloqueada para sempre esperando sua vez, mesmo
 * que a RC esteja livre. Uma thread lenta ou encerrada pode bloquear a outra.
 *
 * Exemplo do problema:
 *   - turn = 0 (vez de T0)
 *   - T0 termina suas iterações e encerra
 *   - T1 está esperando turn == 1, mas ninguém mais vai mudar turn
 *   - T1 fica em loop infinito → DEADLOCK / STARVATION
 *
 * Nesta implementação, como ambas as threads fazem o mesmo número de
 * iterações, o problema não aparece claramente — mas com iterações
 * diferentes ou se uma thread terminar antes, o travamento ocorre.
 *
 * Compile: gcc solucao3_turn.c -o solucao3_turn -lpthread
 */

#include <stdio.h>
#include <pthread.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int saldo = 0;
int turn  = 0; /* 0 = vez da thread 0, 1 = vez da thread 1 */

void* deposito(void* arg)
{
    int id = *((int*) arg); /* ID da thread: 0 ou 1 */

    for (int i = 0; i < REPETICOES; i++)
    {
        /* Espera até ser a sua vez */
        while (turn != id)
            ; /* busy waiting */

        saldo = saldo + 1;  /* região crítica */

        /* Passa a vez para a outra thread */
        turn = 1 - id;
    }
    return NULL;
}

int main()
{
    pthread_t t1, t2;
    int id0 = 0, id1 = 1;

    pthread_create(&t1, NULL, deposito, &id0);
    pthread_create(&t2, NULL, deposito, &id1);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("Saldo final   : %d\n", saldo);
    printf("Valor esperado: %d\n", ESPERADO);
    printf("Resultado correto: %s\n", saldo == ESPERADO ? "SIM" : "NAO");
    printf("NOTA: exclusao mutua garantida, mas independencia NAO.\n");

    return 0;
}
