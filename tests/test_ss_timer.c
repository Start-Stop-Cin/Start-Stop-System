#include "unity.h"
#include "ss_timer.h"
#include "ss_calib.h" // Necessário para acessar SS_AUTOSTOP_TIMEOUT_MS e SS_TASK_PERIOD_MS

// Setup: Executado antes de cada teste
void setUp(void) {
    SS_Timer_Init();
}

void tearDown(void) {
}

// 1. Validação de Inicialização
void test_Timer_Init_ShouldResetInternalState(void) {
    // Forçamos um estado sujo antes de iniciar
    SS_Timer_Run10ms(true);
    
    SS_Timer_Init();

    TEST_ASSERT_EQUAL_UINT32(0, SS_Timer_GetAutostopEventTimeMs());
    TEST_ASSERT_FALSE(SS_Timer_IsAutostopTimeoutReached());
}

// 2. Validação de Acúmulo de Tempo (Caminho Feliz)
void test_Timer_ShouldIncrementTimeWhenAutostopIsActive(void) {
    uint32_t initial_time = SS_Timer_GetAutostopEventTimeMs();

    SS_Timer_Run10ms(true);
    
    // Verifica se incrementou exatamente um período de tarefa (ex: 10ms)
    TEST_ASSERT_EQUAL_UINT32(initial_time + SS_TASK_PERIOD_MS, SS_Timer_GetAutostopEventTimeMs());
    TEST_ASSERT_FALSE(SS_Timer_IsAutostopTimeoutReached());
}

// 3. Validação de Reset Automático
void test_Timer_ShouldResetWhenAutostopIsInactive(void) {
    // Simula tempo acumulado
    for(int i = 0; i < 10; i++) SS_Timer_Run10ms(true);
    TEST_ASSERT_TRUE(SS_Timer_GetAutostopEventTimeMs() > 0);

    // Quando autostop_active for false, deve zerar tudo
    SS_Timer_Run10ms(false);

    TEST_ASSERT_EQUAL_UINT32(0, SS_Timer_GetAutostopEventTimeMs());
    TEST_ASSERT_FALSE(SS_Timer_IsAutostopTimeoutReached());
}

// 4. Validação de Timeout (Borda Superior)
void test_Timer_ShouldReachTimeoutExactlyAtLimit(void) {
    // Calcula quantos passos são necessários para chegar ao timeout
    uint32_t total_steps = SS_AUTOSTOP_TIMEOUT_MS / SS_TASK_PERIOD_MS;

    for(uint32_t i = 0; i < total_steps - 1; i++) {
        SS_Timer_Run10ms(true);
        TEST_ASSERT_FALSE(SS_Timer_IsAutostopTimeoutReached());
    }

    // No passo exato do timeout
    SS_Timer_Run10ms(true);
    TEST_ASSERT_EQUAL_UINT32(SS_AUTOSTOP_TIMEOUT_MS, SS_Timer_GetAutostopEventTimeMs());
    TEST_ASSERT_TRUE(SS_Timer_IsAutostopTimeoutReached());
}

// 5. Validação de Saturação (Anti-Overflow)
void test_Timer_ShouldNotIncrementBeyondTimeout(void) {
    uint32_t total_steps = (SS_AUTOSTOP_TIMEOUT_MS / SS_TASK_PERIOD_MS) + 10;

    for(uint32_t i = 0; i < total_steps; i++) {
        SS_Timer_Run10ms(true);
    }

    // Verifica se o tempo "travou" no timeout e não continuou subindo
    TEST_ASSERT_EQUAL_UINT32(SS_AUTOSTOP_TIMEOUT_MS, SS_Timer_GetAutostopEventTimeMs());
    TEST_ASSERT_TRUE(SS_Timer_IsAutostopTimeoutReached());
}

// 6. Teste de Mutação: Matar mutantes de comparação (cxx_lt_to_le, cxx_ge_to_gt)
void test_Timer_Mutation_Killers(void) {
    // Alvo: if (g_autostop_event_time_ms < SS_AUTOSTOP_TIMEOUT_MS)
    // Se trocarem < por <=, o contador vai para 180010ms. Precisa garantir que para em 180000ms.
    
    uint32_t steps_to_limit = SS_AUTOSTOP_TIMEOUT_MS / SS_TASK_PERIOD_MS;
    
    for(uint32_t i = 0; i <= steps_to_limit + 5; i++) {
        SS_Timer_Run10ms(true);
    }
    
    // Mata mutantes que alteram o limite superior da contagem
    TEST_ASSERT_EQUAL_UINT32(SS_AUTOSTOP_TIMEOUT_MS, SS_Timer_GetAutostopEventTimeMs());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Timer_Init_ShouldResetInternalState);
    RUN_TEST(test_Timer_ShouldIncrementTimeWhenAutostopIsActive);
    RUN_TEST(test_Timer_ShouldResetWhenAutostopIsInactive);
    RUN_TEST(test_Timer_ShouldReachTimeoutExactlyAtLimit);
    RUN_TEST(test_Timer_ShouldNotIncrementBeyondTimeout);
    RUN_TEST(test_Timer_Mutation_Killers);
    return UNITY_END();
}