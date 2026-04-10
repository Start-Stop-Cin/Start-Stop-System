#ifndef SS_FUEL_H
#define SS_FUEL_H

#include <stdbool.h>
#include <stdint.h>

void SS_Fuel_Init(void);
void SS_Fuel_Run10ms(bool ignition_on, bool autostop_active);

uint32_t SS_Fuel_GetAccumulatedAutoStopTimeMs(void);
float SS_Fuel_GetSavedLiters(void);

bool SS_Fuel_DisplayEnabled(void);
bool SS_Fuel_HideEnabled(void);

#endif