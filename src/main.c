#include <stdio.h>
#include "ss_control.h"
#include "ss_inhibit_safety_conditions.h"
#include "ss_operation.h"

SsOperationInputs_t *inputs;
SsOperationOutputs_t *outputs;

// Função de log usando variáveis globais
void print_status() {
    printf("Inputs: [Ign: %d, Btn: %d] | Outputs: [Enabled_Final: %d, LED: %d, Update: %d]\n", 
            g_SS_Inputs.IgnitionStatus, g_SS_Inputs.ButtonInput, 
            g_SS_Outputs.SS_Enabled_Final, g_SS_Outputs.HMI_LED, g_SS_Outputs.LED_Update_Cmd);
}

int main() {
    SS_Init();

    printf("--- Passo 1: Ligando a Ignicao ---\n");
    g_SS_Inputs.IgnitionStatus = true;
    SS_Step();
    print_status();

    printf("\n--- Passo 2: Pressionando o botao (Debounce de 50ms) ---\n");
    g_SS_Inputs.ButtonInput = true; // Requisito FR-12 
    for(int i = 1; i <= 6; i++) {
        SS_Step();
        printf("Ciclo %d - Button_Rise_Valid: %d\n", i, g_SS_Outputs.Button_Rise_Valid);
    }
    
    printf("\n--- Passo 3: Estado com o botao ainda pressionado ---\n");
    print_status();

    printf("\n--- Passo 4: Soltando o botao (Finaliza Toggle e Atualiza LED) ---\n");
    g_SS_Inputs.ButtonInput = false;
    SS_Step(); // Ciclo extra para processar a saída do estado Pressed no Stateflow 
    print_status();

    while (true)
    {

        bool * ss_inhibit_wrapper_retVal;
        ss_inhibit_wrapper(
            inputs->inclination_angle, inputs->door_status, inputs->seatbelt_status, inputs->gear_position, inputs->ss_enable,
            outputs->autostop_active, inputs->brake_pressed,
            inputs->ignition_on, outputs->engine_stop_request, inputs->vehicle_speed_kmh,
            ss_inhibit_wrapper_retVal 
        );
        inputs->autostop_allowed = ss_inhibit_wrapper_retVal[0];

        SS_Operation_Run10ms( inputs, outputs );
    }

    return 0;
}