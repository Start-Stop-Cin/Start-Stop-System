#include "ss_input_proc.h"
#include "ss_calib.h"

void SS_InputProc_Run(const SsRawInputs_t *raw, SsProcessedInputs_t *proc) {
	if ((raw != NULL) && (proc != NULL)) {
		proc->ignition_on = raw->ignition_on;
		proc->vehicle_speed_kmh = raw->vehicle_speed_kmh;
		proc->door_closed = raw->door_closed;
		proc->seatbelt_fastened = raw->seatbelt_fastened;
		proc->button_input = raw->button_input;
		proc->inclination_angle_rad = raw->inclination_angle_rad;
		proc->gear_position = raw->gear_position;

		proc->brake_pressed =
		    (raw->brake_pedal_position_pct >= SS_BRAKE_PRESSED_THRESHOLD_PCT);

		proc->accelerator_pressed = (raw->accelerator_pedal_position_pct >=
		                             SS_ACCEL_PRESSED_THRESHOLD_PCT);

		proc->gear_in_drive = (raw->gear_position == SS_GEAR_D);

		proc->speed_below_autostop_limit =
		    (raw->vehicle_speed_kmh <= SS_AUTOSTOP_SPEED_MAX_KMH);

		proc->speed_above_drivecycle_threshold =
		    (raw->vehicle_speed_kmh > SS_DRIVECYCLE_SPEED_THRESHOLD_KMH);
	}
}