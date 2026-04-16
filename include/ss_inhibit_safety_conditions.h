#ifndef SS_INHIBIT_SAFETY_CONDITIONS_H
#define SS_INHIBIT_SAFETY_CONDITIONS_H

#include <stdbool.h>
#include <ss_types.h>

bool inhibit_safety_conditions(float InclinationAngle, bool DoorStatus,
                               bool SeatbeltStatus,
                               SsGearPosition_t GearPosition, bool SS_Enabled);

bool standstill_management(bool AutoStopActive, bool BrakeStatus,
                           bool DoorStatus, bool SeatbeltStatus);

bool drivecycle_memory_block(bool IgnitionStatus, bool EngineStopRequest,
                             float VehicleSpeed);

typedef struct {
	bool Q;
} sr_state_t;

#endif