#ifndef SS_CONTROL_H
#define SS_CONTROL_H

#include <stdint.h>
#include <stdbool.h>

// Inputs
typedef struct {
    bool ButtonInput;      
    bool IgnitionStatus;   
} SS_Inputs_t;

// Outputs
typedef struct {
    bool SS_Enabled_Final; // Porta 1
    bool HMI_LED;          // Porta 2        
    bool LED_Update_Cmd;   // Porta 3  
    bool Button_Rise_Valid;// Porta 4
    bool SS_Enabled;       // Porta 5   
    bool SS_Enable_prev;   // Porta [SS_Enable_prev]
} SS_Outputs_t;

// Máquina de estados para o debounce do botão
typedef enum {
    ST_IDLE,
    ST_DEBOUNCING,
    ST_PRESSED,
    ST_WAIT_RELEASE
} SS_FSM_State_t;

typedef struct {
    SS_FSM_State_t fsm_state;
    uint8_t cnt;
    bool ss_enable_current;
    bool ignition_prev;    
} SS_State_t;

// --- VARIÁVEIS GLOBAIS PRA INTEGRAÇÃO ---

extern SS_Inputs_t  g_SS_Inputs;
extern SS_Outputs_t g_SS_Outputs;
extern SS_State_t   g_SS_State;

void SS_Init(void);
void SS_Step(void);

// --- NOVAS FUNÇÕES DE ABSTRAÇÃO (APIs) ---
void press_SS_button(void);   // Abstrai o Passo 2
void release_SS_button(void); // Abstrai o Passo 4

#endif
