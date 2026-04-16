#ifndef SS_INPUT_PROC_H
#define SS_INPUT_PROC_H

#include <stdbool.h>
#include "ss_types.h"

typedef struct {
	bool ignition_on;
	float vehicle_speed_kmh;
	float brake_pedal_position_pct;
	float accelerator_pedal_position_pct;
	SsGearPosition_t gear_position;
	bool door_closed;
	bool seatbelt_fastened;
	bool button_input;
	float inclination_angle_rad;
} SsRawInputs_t;

typedef struct {
	bool ignition_on;
	float vehicle_speed_kmh;
	bool brake_pressed;
	bool accelerator_pressed;
	bool gear_in_drive;
	bool door_closed;
	bool seatbelt_fastened;
	bool button_input;
	bool speed_below_autostop_limit;
	bool speed_above_drivecycle_threshold;
	float inclination_angle_rad;
	SsGearPosition_t gear_position;
} SsProcessedInputs_t;

void SS_InputProc_Run(const SsRawInputs_t *raw, SsProcessedInputs_t *proc);

#endif