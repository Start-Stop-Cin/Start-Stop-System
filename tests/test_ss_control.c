#include "unity.h"
#include "ss_control.h"

// Função executada antes de cada teste individual
void setUp(void) {
    SS_Init(); 
}

// Função executada após cada teste individual
void tearDown(void) {
    
}

// 1. Validação do Estado Inicial (Reset)
void test_SS_Init_Should_InitializeGlobalStructures(void) {
    TEST_ASSERT_EQUAL(ST_IDLE, g_SS_State.fsm_state);
    TEST_ASSERT_EQUAL_UINT8(0, g_SS_State.cnt);
    TEST_ASSERT_TRUE(g_SS_State.ss_enable_current);
    TEST_ASSERT_FALSE(g_SS_Inputs.ButtonInput);
    TEST_ASSERT_FALSE(g_SS_Inputs.IgnitionStatus);
}

// 2. Validação de Robustez: Debounce de Curta Duração (Ruído)
void test_ButtonDebounce_ShouldIgnoreSpikesShorterThan50ms(void) {
    g_SS_Inputs.IgnitionStatus = true;
    SS_Step(); // Estabiliza ignição (borda de subida processada)

    press_SS_Button();
    // Simula 3 ciclos (30ms se o step for 10ms)
    for(int i = 0; i < 3; i++) {
        SS_Step();
        TEST_ASSERT_EQUAL(ST_DEBOUNCING, g_SS_State.fsm_state);
        TEST_ASSERT_FALSE(g_SS_Outputs.Button_Rise_Valid);
    }
    
    // Solta o botão antes de completar 5 ciclos
    release_SS_Button();
    SS_Step();
    
    TEST_ASSERT_EQUAL(ST_IDLE, g_SS_State.fsm_state);
    TEST_ASSERT_TRUE(g_SS_State.ss_enable_current); // Sistema deve permanecer ligado
}

// 3. Validação de Funcionalidade: Toggle Completo
void test_ButtonToggle_ShouldChangeStateAfterValidPress(void) {
    g_SS_Inputs.IgnitionStatus = true;
    SS_Step(); // Estabiliza ignição

    press_SS_Button();
    
    bool valid_detected = false;

    // Rodar os ciclos e monitorar se o Valid aparece em algum momento
    for(int i = 0; i < 10; i++) {
        SS_Step();
        if (g_SS_Outputs.Button_Rise_Valid) {
            valid_detected = true;
        }
    }

    TEST_ASSERT_TRUE(valid_detected);               // Verifica se o pulso ocorreu
    TEST_ASSERT_FALSE(g_SS_State.ss_enable_current); // Verifica se inverteu o estado
    TEST_ASSERT_TRUE(g_SS_Outputs.HMI_LED);          // Verifica o LED
}

// 4. Validação de Segurança: Reset por Ignição (Requisito Primário)
void test_IgnitionRisingEdge_ShouldAlwaysEnableSS(void) {
    // Forçamos um estado onde o sistema está desligado
    g_SS_State.ss_enable_current = false;
    g_SS_Inputs.IgnitionStatus = false;
    g_SS_State.ignition_prev = false;

    // Simula o motorista ligando a chave
    g_SS_Inputs.IgnitionStatus = true;
    SS_Step();

    TEST_ASSERT_TRUE(g_SS_State.ss_enable_current); // Reset para ON
    TEST_ASSERT_TRUE(g_SS_Outputs.SS_Enabled_Final);
    TEST_ASSERT_FALSE(g_SS_Outputs.HMI_LED);        // LED apaga (Sistema ON)
}

void test_Mutation_Killers(void) {
    
    SS_Init();
    g_SS_Inputs.ButtonInput = true;
    for(int i = 0; i < 4; i++) SS_Step(); // 4 ciclos
    TEST_ASSERT_FALSE(g_SS_State.ss_enable_current); 
    
    SS_Step(); // 5º ciclo EXATO
    
    TEST_ASSERT_TRUE(g_SS_State.ss_enable_current != false); 

    bool state_before = g_SS_State.ss_enable_current;
    
    g_SS_Inputs.ButtonInput = true;
    for(int i = 0; i < 6; i++) SS_Step(); 
    
    if (g_SS_State.ss_enable_current != state_before) {
        TEST_ASSERT_TRUE(g_SS_Outputs.LED_Update_Cmd);
    }
}

void test_SS_Enabled_Final_Logic(void) {
    // Caso 1: SS Habilitado mas SEM Ignição -> Final deve ser FALSE
    SS_Init();
    g_SS_Inputs.IgnitionStatus = false;
    g_SS_State.ignition_prev = false;
    g_SS_State.ss_enable_current = true;
    SS_Step();
    TEST_ASSERT_FALSE(g_SS_Outputs.SS_Enabled_Final);

    // Caso 2: SS Desabilitado mas COM Ignição estável -> Final deve ser FALSE
    SS_Init();
    g_SS_Inputs.IgnitionStatus = true;
    g_SS_State.ignition_prev = true; // Forcei que já estava ligado para evitar borda de subida
    g_SS_State.ss_enable_current = false;
    SS_Step();
    TEST_ASSERT_FALSE(g_SS_Outputs.SS_Enabled_Final);
}   

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SS_Init_Should_InitializeGlobalStructures);
    RUN_TEST(test_ButtonDebounce_ShouldIgnoreSpikesShorterThan50ms);
    RUN_TEST(test_ButtonToggle_ShouldChangeStateAfterValidPress);
    RUN_TEST(test_IgnitionRisingEdge_ShouldAlwaysEnableSS);
    RUN_TEST(test_SS_Enabled_Final_Logic);
    RUN_TEST(test_Mutation_Killers);
    return UNITY_END();
}
