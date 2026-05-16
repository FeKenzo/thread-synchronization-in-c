/*
 * SOLUÇÃO 4 — Algoritmo de Peterson
 *
 * Combina as ideias de "intenção" (want[]) e "alternância" (turn) para
 * garantir exclusão mútua, progresso e independência entre as threads.
 *
 * VARIÁVEIS:
 *   want[i] = 1  → thread i deseja entrar na região crítica
 *   turn         → em caso de conflito, indica quem cede a vez
 *
 * PROTOCOLO DE ENTRADA (thread i, onde j = 1 - i):
 *   1. want[i] = 1       → declara intenção de entrar
 *   2. turn = j          → cede a vez para a outra thread (cortesia)
 *   3. while (want[j] && turn == j) → espera se: a outra quer entrar E
 *                                     a vez é da outra
 *
 * POR QUE FUNCIONA?
 *   - Exclusão mútua: se ambas querem entrar, "turn" só pode ser 0 ou 1,
 *     então apenas uma passa pelo while. A outra espera.
 *   - Progresso: se apenas uma quer entrar, ela entra imediatamente
 *     (want[j] == 0, o while não bloqueia).
 *   - Independência: uma thread só bloqueia a outra se realmente quiser
 *     a RC. Se não quiser (want[i] == 0), não interfere.
 *   - Sem starvation: a variável turn garante que nenhuma thread espera
 *     indefinidamente — a outra sempre vai liberar e ceder a vez.
 *   - Independe de temporização: a correção é puramente lógica, não
 *     depende da velocidade relativa das threads.
 *
 * Compile: gcc solucao4_peterson.c -o solucao4_peterson -lpthread
 */

#include <stdio.h>
#include <pthread.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int saldo   = 0;
int want[2] = {0, 0}; /* intenção de entrar na RC */
int turn    = 0;       /* quem cede em caso de conflito */

void* deposito(void* arg)
{
    int i = *((int*) arg); /* ID desta thread: 0 ou 1 */
    int j = 1 - i;         /* ID da outra thread       */

    for (int k = 0; k < REPETICOES; k++)
    {
        /* --- Protocolo de ENTRADA --- */
        want[i] = 1;          /* declara intenção               */
        turn    = j;          /* cede a vez para a outra thread */

        /* Espera enquanto a outra quer entrar E a vez é dela */
        while (want[j] == 1 && turn == j)
            ; /* busy waiting — mas com garantia de progresso */

        /* --- Região Crítica --- */
        saldo = saldo + 1;

        /* --- Protocolo de SAÍDA --- */
        want[i] = 0; /* declara que saiu da RC */
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
    printf("NOTA: Peterson garante EM, progresso e independencia.\n");

    return 0;
}
