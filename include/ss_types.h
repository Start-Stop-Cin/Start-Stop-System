#ifndef SS_TYPES_H
#define SS_TYPES_H


#include <stdbool.h>
#include <stdint.h>

// defines the possible gear positions for the start-stop system
typedef enum
{
    SS_GEAR_P = 0,
    SS_GEAR_R = 1,
    SS_GEAR_N = 2,
    SS_GEAR_D = 3
} SsGearPosition_t;


// Defines the possible states of the vehicle's operation for the start-stop system
typedef enum
{
    SS_OP_STATE_VEHICLE_OFF = 0,
    SS_OP_STATE_ENGINE_RUNNING = 1,
    SS_OP_STATE_AUTOSTOP_ACTIVE = 2
} SsOperationState_t;

#endif



