/*
 * PROBLEMA DA CONCORRÊNCIA — Condição de Disputa (Race Condition)
 *
 * Duas threads incrementam a variável compartilhada "saldo" sem qualquer
 * sincronização. O resultado final é variável e incorreto.
 *
 * POR QUE OCORRE A RACE CONDITION?
 * A operação "saldo = saldo + 1" não é atômica. Ela é composta por três
 * instruções de baixo nível:
 *   1. LOAD:  lê o valor de "saldo" da memória para um registrador
 *   2. ADD:   soma 1 ao valor no registrador
 *   3. STORE: escreve o resultado de volta na memória
 *
 * Se o SO troca o contexto entre essas instruções, ambas as threads podem
 * ler o mesmo valor de "saldo", incrementar separadamente e sobrescrever
 * o trabalho uma da outra — perdendo incrementos.
 *
 * CONDIÇÕES DE BERNSTEIN:
 * Para que duas operações P1 e P2 possam ser executadas em paralelo com
 * segurança, as seguintes condições devem ser satisfeitas:
 *   1. W(P1) ∩ W(P2) = {}  → os conjuntos de escrita não se intersectam
 *   2. R(P1) ∩ W(P2) = {}  → P2 não escreve onde P1 lê
 *   3. W(P1) ∩ R(P2) = {}  → P1 não escreve onde P2 lê
 *
 * Neste programa:
 *   - R(deposito) = { saldo }  → ambas as threads leem "saldo"
 *   - W(deposito) = { saldo }  → ambas as threads escrevem em "saldo"
 *
 * As condições 1, 2 e 3 são todas VIOLADAS, pois ambas leem e escrevem
 * na mesma variável. Portanto, a execução paralela NÃO é segura.
 *
 * Compile: gcc problema_concorrencia.c -o problema_concorrencia -lpthread
 * Execute: ./problema_concorrencia  (execute várias vezes e observe a variação)
 */

#include <stdio.h>
#include <pthread.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int saldo = 0;

void* deposito(void* arg)
{
    for (int i = 0; i < REPETICOES; i++)
    {
        saldo = saldo + 1; /* região crítica SEM proteção */
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

    printf("Saldo final  : %d\n", saldo);
    printf("Valor esperado: %d\n", ESPERADO);
    printf("Incrementos perdidos: %d\n", ESPERADO - saldo);

    return 0;
}
