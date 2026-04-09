#include <stdio.h>
#include "ss_control.h"

// Função de log usando variáveis globais
void print_status() {
    printf("Outputs: [Enabled_Final: %d, LED: %d, Update: %d]\n", 
            g_SS_Outputs.SS_Enabled_Final, g_SS_Outputs.HMI_LED, g_SS_Outputs.LED_Update_Cmd);
}

int main() {
    SS_Init();

    printf("--- Passo 1: Ligando a Ignicao ---\n");
    g_SS_Inputs.IgnitionStatus = true;
    SS_Step();
    print_status();

    printf("\n--- Passo 2: Simulando Pressionamento do Botao ---\n");
   
    for(int i = 1; i <= 6; i++) {
        press_SS_button(); 
        SS_Step(); // O tempo passa aqui
        printf("Ciclo %d - Valid: %d\n", i, g_SS_Outputs.Button_Rise_Valid);
    }
    
    printf("\n--- Passo 3: Estado com o botao ainda preso ---\n");
    print_status();

    printf("\n--- Passo 4: Soltando o botao ---\n");
    release_SS_button(); // coloca false e atualiza
    print_status();

    return 0;
}