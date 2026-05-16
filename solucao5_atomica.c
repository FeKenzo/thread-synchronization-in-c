/*
 * SOLUÇÃO 5 — Operações Atômicas (Test-and-Set / XCHG)
 *
 * Utiliza a instrução atômica atomic_exchange() da biblioteca <stdatomic.h>,
 * que corresponde à instrução XCHG (exchange) do hardware x86.
 *
 * COMO FUNCIONA O TEST-AND-SET / XCHG?
 * A operação atomic_exchange(&lock, 1) faz em UMA instrução indivisível:
 *   1. Lê o valor atual de "lock"
 *   2. Escreve 1 em "lock"
 *   3. Retorna o valor antigo
 *
 * Por ser indivisível no nível de hardware, o processador garante que
 * nenhuma outra thread pode executar essa operação ao mesmo tempo.
 * O barramento de memória é bloqueado durante a instrução.
 *
 * PROTOCOLO:
 *   - Se atomic_exchange retorna 0: lock estava livre → entrou na RC
 *   - Se atomic_exchange retorna 1: lock estava ocupado → continua esperando
 *
 * VANTAGENS:
 *   - Exclusão mútua garantida pelo hardware
 *   - Independe do número de threads ou processadores
 *   - Operação simples e eficiente no nível de núcleo do SO
 *
 * DESVANTAGEM:
 *   - Ainda usa espera ocupada (busy waiting) — consome CPU enquanto aguarda
 *   - Pode causar starvation em sistemas com muita contenção
 *
 * POR QUE É USADO NO NÚCLEO DO SO?
 * O kernel não pode usar mutexes (que dependem do próprio SO para funcionar).
 * Para proteger estruturas internas como filas de escalonamento e tabelas de
 * processos, o kernel usa spinlocks baseados em Test-and-Set, que são rápidos
 * e não dependem de chamadas de sistema. A espera ocupada é aceitável nesse
 * contexto porque as regiões críticas do kernel são muito curtas.
 *
 * Compile: gcc solucao5_atomica.c -o solucao5_atomica -lpthread
 */

#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int           saldo = 0;
atomic_int    lock  = 0; /* 0 = livre, 1 = ocupado */

void* deposito(void* arg)
{
    for (int i = 0; i < REPETICOES; i++)
    {
        /* --- Protocolo de ENTRADA (Test-and-Set / XCHG) --- */
        /* Tenta adquirir o lock atomicamente.
         * Enquanto retorna 1 (ocupado), fica em espera ocupada. */
        while (atomic_exchange(&lock, 1) == 1)
            ; /* spinlock — busy waiting com garantia de atomicidade */

        /* --- Região Crítica --- */
        saldo = saldo + 1;

        /* --- Protocolo de SAÍDA --- */
        atomic_store(&lock, 0); /* libera o lock atomicamente */
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
    printf("NOTA: atomico garante EM, mas usa busy waiting.\n");

    return 0;
}
