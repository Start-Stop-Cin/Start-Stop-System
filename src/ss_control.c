#include "ss_control.h"

// Definição das variáveis globais
SS_Inputs_t  g_SS_Inputs;
SS_Outputs_t g_SS_Outputs;
SS_State_t   g_SS_State;

void SS_Init(void) {
    g_SS_State.fsm_state = ST_IDLE;
    g_SS_State.cnt = 0;
    g_SS_State.ss_enable_current = true; // Por padrão, Start-Stop inicia Habilitado
    g_SS_State.ignition_prev = false;
    
    // Inicializa entradas e saídas
    g_SS_Inputs.ButtonInput = false;
    g_SS_Inputs.IgnitionStatus = false;
}

void SS_Step(void) {
    // Memória local do estado anterior (Bloco Unit Delay 1/z) 
    bool ss_enable_prev = g_SS_State.ss_enable_current;
    
    // 1. Botão Debounce (Stateflow) 
    g_SS_Outputs.Button_Rise_Valid = false;
    switch (g_SS_State.fsm_state) {
        case ST_IDLE:
            if (g_SS_Inputs.ButtonInput) {
                g_SS_State.fsm_state = ST_DEBOUNCING;
                g_SS_State.cnt = 0;
            }
            break;
        case ST_DEBOUNCING:
            if (g_SS_Inputs.ButtonInput) {
                g_SS_State.cnt++;
                if (g_SS_State.cnt >= 5) {
                    g_SS_State.fsm_state = ST_PRESSED;
                    g_SS_Outputs.Button_Rise_Valid = true; // Dispara aqui!
                }
            } else {
                g_SS_State.cnt = 0;
                g_SS_State.fsm_state = ST_IDLE;
            }
            break;
        case ST_PRESSED:
            // serve apenas para esperar o fluxo seguir
            g_SS_State.fsm_state = ST_WAIT_RELEASE;
            break;
        case ST_WAIT_RELEASE:
            if (!g_SS_Inputs.ButtonInput) {
                g_SS_State.fsm_state = ST_IDLE;
            }
            break;
    }

    // 2. Ignition Status Rising Edge Detection
    bool ignition_rising_edge = (g_SS_Inputs.IgnitionStatus && !g_SS_State.ignition_prev);
    g_SS_State.ignition_prev = g_SS_Inputs.IgnitionStatus;

    // 3. SW_Toggle & SW_IgnitionInit
    bool sw_toggle_out;
    if (g_SS_Outputs.Button_Rise_Valid) {
        sw_toggle_out = !ss_enable_prev; // Inverte o estado anterior
    } else {
        sw_toggle_out = ss_enable_prev;  // Mantém o estado anterior
    }

    if (ignition_rising_edge) {
        g_SS_State.ss_enable_current = true; 
    } else {
        g_SS_State.ss_enable_current = sw_toggle_out;
    }

    // 4. Saídas e Máscaras
    g_SS_Outputs.SS_Enable_prev = ss_enable_prev;
    g_SS_Outputs.SS_Enabled = g_SS_State.ss_enable_current;
    g_SS_Outputs.SS_Enabled_Final = g_SS_State.ss_enable_current && g_SS_Inputs.IgnitionStatus;
    g_SS_Outputs.HMI_LED = !g_SS_State.ss_enable_current;
    g_SS_Outputs.LED_Update_Cmd = (g_SS_State.ss_enable_current != ss_enable_prev);
}

void press_SS_button(void) {
    g_SS_Inputs.ButtonInput = true;
	SS_Step();
}

void release_SS_button(void) {
    g_SS_Inputs.ButtonInput = false;
	SS_Step();
}