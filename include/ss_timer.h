#ifndef SS_TIMER_H
#define SS_TIMER_H

#include <stdbool.h>
#include <stdint.h>

void SS_Timer_Init(void);
void SS_Timer_Run10ms(bool autostop_active);

bool SS_Timer_IsAutostopTimeoutReached(void);
uint32_t SS_Timer_GetAutostopEventTimeMs(void);

#endif
