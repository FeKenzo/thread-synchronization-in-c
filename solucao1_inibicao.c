/*
 * SOLUÇÃO 1 — Inibição de Interrupções (Simulada)
 *
 * Em sistemas reais, o núcleo do SO desabilita interrupções antes de entrar
 * na região crítica, impedindo o escalonador de trocar o contexto durante
 * a execução da seção crítica.
 *
 * Como não é possível desabilitar interrupções reais em user space no Linux,
 * simulamos o comportamento usando um mutex do próprio SO — que internamente
 * usa esse mecanismo. Para fins didáticos, as funções foram renomeadas para
 * refletir o conceito de "inibir/restaurar interrupções".
 *
 * POR QUE É INVIÁVEL EM SISTEMAS MULTIPROCESSADOS?
 * Em um sistema com múltiplos núcleos (multicore), desabilitar interrupções
 * em um núcleo NÃO impede que outro núcleo continue executando threads
 * concorrentemente. Assim, a exclusão mútua não é garantida, pois outra CPU
 * pode acessar a região crítica ao mesmo tempo. Além disso:
 *   - Desabilitar interrupções em todos os núcleos é muito custoso.
 *   - Pode afetar a responsividade do sistema (interrupções de hardware
 *     como teclado, rede e disco também seriam bloqueadas).
 *   - É um privilégio exclusivo do núcleo do SO — código de usuário não
 *     pode executar essa instrução (resultaria em falha de proteção).
 *
 * Compile: gcc solucao1_inibicao.c -o solucao1_inibicao -lpthread
 */

#include <stdio.h>
#include <pthread.h>

#define REPETICOES 100000
#define ESPERADO   (REPETICOES * 2)

int saldo = 0;

/* Simulação da estrutura de controle de interrupções */
pthread_mutex_t interrupcoes = PTHREAD_MUTEX_INITIALIZER;

/* Simula a instrução CLI (Clear Interrupt Flag) — desabilita interrupções */
void inibir_interrupcoes()
{
    pthread_mutex_lock(&interrupcoes);
}

/* Simula a instrução STI (Set Interrupt Flag) — restaura interrupções */
void restaurar_interrupcoes()
{
    pthread_mutex_unlock(&interrupcoes);
}

void* deposito(void* arg)
{
    for (int i = 0; i < REPETICOES; i++)
    {
        inibir_interrupcoes();      /* --- entrada na região crítica --- */

        saldo = saldo + 1;          /* região crítica protegida          */

        restaurar_interrupcoes();   /* --- saída da região crítica  --- */
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

    pthread_mutex_destroy(&interrupcoes);
    return 0;
}
