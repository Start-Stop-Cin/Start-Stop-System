#include "ss_app.h"
#include "ss_operation.h"
#include "ss_timer.h"
#include "ss_input_proc.h"
#include "ss_inhibit_safety_conditions.h"
#include "ss_types.h"
#include "co2.h"
#include "ss_control.h"
#include "ss_fuel.h"

#include <stddef.h>

static bool g_prev_engine_stop_request = false;

void SS_App_Init(void) {
	SS_Init(); // Initalizes the Start Stop button debounce
	SS_Operation_Init();
	SS_Timer_Init();
	Co2_Init();
	SS_Fuel_Init();

	g_prev_engine_stop_request = false;
}

void SS_App_Run10ms_If(
    bool ignition_on, float brake_pedal_position_pct,
    float accelerator_pedal_position_pct, int gear_position,
    float vehicle_speed_kmh, bool door_status, bool seatbelt_status,
    bool button_input, float inclination_angle_rad, bool *ss_enable,
    bool *hmi_led, bool *led_update_cmd, bool *button_rise_valid,
    bool *autostop_allowed, bool *safe_stop, bool *autostop_active,
    bool *engine_stop_request, bool *engine_restart_request, int *state,
    unsigned int *autostop_event_time_ms, bool *autostop_timeout_reached,
    unsigned int *autostop_accumulated_time_ms, float *co2_avoided_g,
    bool *display_co2, bool *hide_co2,
    unsigned int *autostop_accumulated_time_ms_fuel, float *fuel_saved_l,
    bool *display_fuel, bool *hide_fuel) {
	SsRawInputs_t raw;
	SsProcessedInputs_t proc;
	SsOperationInputs_t op_in;
	SsOperationOutputs_t op_out;

	bool safety_ok = false;
	bool drivecycle_ok = false;
	bool autostop_allowed_local = false;
	bool safe_stop_local = false;

	/*======================================================
     * 1. Build raw inputs
     *======================================================*/
	raw.ignition_on = ignition_on;
	raw.vehicle_speed_kmh = vehicle_speed_kmh;
	raw.brake_pedal_position_pct = brake_pedal_position_pct;
	raw.accelerator_pedal_position_pct = accelerator_pedal_position_pct;
	raw.gear_position = (SsGearPosition_t)gear_position;
	raw.door_closed = door_status;
	raw.seatbelt_fastened = seatbelt_status;
	raw.button_input = button_input;
	raw.inclination_angle_rad = inclination_angle_rad;

	/*======================================================
     * 2. Process button input
     *======================================================*/

	g_SS_Inputs.ButtonInput = button_input;
	g_SS_Inputs.IgnitionStatus = ignition_on;

	SS_Step();

	bool ss_enable_local = g_SS_Outputs.SS_Enabled_Final;

	/*======================================================
     * 3. Process raw inputs
     *======================================================*/
	SS_InputProc_Run(&raw, &proc);

	/*======================================================
     * 4. Inhibit logic - preliminary conditions
     *    Drive cycle uses previous cycle engine stop request
     *======================================================*/
	safety_ok = inhibit_safety_conditions(inclination_angle_rad, door_status,
	                                      seatbelt_status, gear_position,
	                                      ss_enable_local);

	drivecycle_ok = drivecycle_memory_block(
	    ignition_on, g_prev_engine_stop_request, vehicle_speed_kmh);

	autostop_allowed_local = safety_ok && drivecycle_ok;

	/*======================================================
    * 5. Safe stop based on CURRENT operation state
    *======================================================*/
	safe_stop_local =
	    standstill_management(SS_Operation_IsAutoStopActiveState(),
	                          proc.brake_pressed, door_status, seatbelt_status);

	/*======================================================
     * 6. Build operation inputs
     *======================================================*/
	op_in.ignition_on = proc.ignition_on;
	op_in.ss_enable = ss_enable_local;
	op_in.autostop_allowed = autostop_allowed_local;
	op_in.brake_pressed = proc.brake_pressed;
	op_in.accelerator_pressed = proc.accelerator_pressed;
	op_in.gear_position = proc.gear_position;
	op_in.vehicle_speed_kmh = proc.vehicle_speed_kmh;
	op_in.autostop_timeout_reached = SS_Timer_IsAutostopTimeoutReached();

	/*======================================================
     * 7. Run operation module
     *======================================================*/
	SS_Operation_Run10ms(&op_in, &op_out, safe_stop_local);

	/*======================================================
     * 8. Update timer with actual autostop state
     *======================================================*/
	SS_Timer_Run10ms(op_out.autostop_active);

	/*======================================================
     * 9. Run CO2 module to update avoided CO2 and display flags based on autostop state
     *======================================================*/
	Co2_Run10ms(op_in.ignition_on, op_out.autostop_active);

	SS_Fuel_Run10ms(op_in.ignition_on, op_out.autostop_active);

	/*======================================================
     * 10. Save previous-cycle signals needed by memory blocks
     *======================================================*/
	g_prev_engine_stop_request = op_out.engine_stop_request;

	/*======================================================
     * 11. Outputs to Simulink
     *======================================================*/
	if (autostop_allowed != NULL) {
		*autostop_allowed = autostop_allowed_local;
	}

	if (safe_stop != NULL) {
		*safe_stop = safe_stop_local;
	}

	if (autostop_active != NULL) {
		*autostop_active = op_out.autostop_active;
	}

	if (engine_stop_request != NULL) {
		*engine_stop_request = op_out.engine_stop_request;
	}

	if (engine_restart_request != NULL) {
		*engine_restart_request = op_out.engine_restart_request;
	}

	if (state != NULL) {
		*state = (int)op_out.state;
	}

	if (autostop_event_time_ms != NULL) {
		*autostop_event_time_ms = SS_Timer_GetAutostopEventTimeMs();
	}

	if (autostop_timeout_reached != NULL) {
		*autostop_timeout_reached = SS_Timer_IsAutostopTimeoutReached();
	}

	if (autostop_accumulated_time_ms != NULL) {
		*autostop_accumulated_time_ms = Co2_GetAccumulatedAutoStopTimeMs();
	}

	if (co2_avoided_g != NULL) {
		*co2_avoided_g = Co2_GetAvoidedGrams();
	}

	if (display_co2 != NULL) {
		*display_co2 = Co2_DisplayEnabled();
	}

	if (hide_co2 != NULL) {
		*hide_co2 = Co2_HideEnabled();
	}

	if (ss_enable != NULL) {
		*ss_enable = g_SS_Outputs.SS_Enabled_Final;
	}

	if (hmi_led != NULL) {
		*hmi_led = g_SS_Outputs.HMI_LED;
	}

	if (led_update_cmd != NULL) {
		*led_update_cmd = g_SS_Outputs.LED_Update_Cmd;
	}

	if (button_rise_valid != NULL) {
		*button_rise_valid = g_SS_Outputs.Button_Rise_Valid;
	}

	if (autostop_accumulated_time_ms_fuel != NULL) {
		*autostop_accumulated_time_ms_fuel =
		    SS_Fuel_GetAccumulatedAutoStopTimeMs();
	}

	if (fuel_saved_l != NULL) {
		*fuel_saved_l = SS_Fuel_GetSavedLiters();
	}

	if (display_fuel != NULL) {
		*display_fuel = SS_Fuel_DisplayEnabled();
	}

	if (hide_fuel != NULL) {
		*hide_fuel = SS_Fuel_HideEnabled();
	}
}
