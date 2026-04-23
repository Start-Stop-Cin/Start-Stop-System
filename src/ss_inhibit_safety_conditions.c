#include "ss_inhibit_safety_conditions.h"
#include <stdbool.h>
#include <stddef.h>
#include <ss_calib.h>
#include <ss_types.h>

static static bool sr_flip_flop(bool S, bool R, sr_state_t *state);

// Checks if all safety conditions are met to allow autostop based on the inclination angle, door status, seatbelt status, gear position, and whether the start-stop system is enabled.
bool inhibit_safety_conditions(float InclinationAngle, bool DoorStatus,
                               bool SeatbeltStatus,
                               SsGearPosition_t GearPosition, bool SS_Enabled) {
	bool is_allowed = false;

	if ((InclinationAngle <= SS_INCLINATION_MAX_RAD) &&
	    (GearPosition == SS_GEAR_D) && (DoorStatus == true) &&
	    (SeatbeltStatus == true) && (SS_Enabled == true)) {
		is_allowed = true;
	}

	return is_allowed;
}

//  Check if safe-stop is reachable
bool standstill_management(bool AutoStopActive, bool BrakeStatus,
                           bool DoorStatus, bool SeatbeltStatus) {

	static sr_state_t flipFlopState = {0};
	static bool Q_d = false;

	bool setValue = AutoStopActive && (!DoorStatus || !SeatbeltStatus);
	bool resetValue = BrakeStatus && DoorStatus && SeatbeltStatus;

	bool Q = sr_flip_flop(setValue, resetValue, &flipFlopState);
	bool SafeStop = Q_d;

	Q_d = Q;

	return SafeStop;
}

bool drivecycle_memory_block(bool IgnitionStatus, bool EngineStopRequest,
                             float VehicleSpeed) {
	static bool y_1 = false;

	bool or1 = ((!IgnitionStatus) || EngineStopRequest);
	bool or2 = ((VehicleSpeed > 8.0f) || y_1);
	bool y = or1 ? false : or2;

	y_1 = y;

	return y;
}

static bool sr_flip_flop(bool S, bool R, sr_state_t *state) {
	bool q_out = false;

	if (state != NULL) {
		if ((S == true) && (R == false)) {
			state->Q = true;
		} else if ((S == false) && (R == true)) {
			state->Q = false;
		} else {
			/* Hold previous state for:
               - S == 0, R == 0
               - S == 1, R == 1 (invalid condition) */
		}

		q_out = state->Q;
	}

	return q_out;
}
