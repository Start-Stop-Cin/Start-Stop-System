
#include "ss_operation.h"
#include "ss_calib.h"
#include <stddef.h>

// Global variable to hold the current state of the start-stop system operation - initialized to VEHICLE_OFF
static SsOperationState_t g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;

static bool SS_StopIgnitionOn(
    const SsOperationInputs_t *
        in) // Helper function to check if the ignition is on based on the inputs
{
	return (in != NULL) && (in->ignition_on == true);
}

static bool SS_StopEnabled(
    const SsOperationInputs_t *
        in) //  Helper function to check if the start-stop system is enabled by the user based on the inputs
{
	return (in != NULL) && (in->ss_enable == true);
}

static bool SS_StopAllowed(
    const SsOperationInputs_t *
        in) // Helper function to check if the current conditions allow for autostop based on the inputs
{
	return (in != NULL) && (in->autostop_allowed == true);
}

static bool SS_StopGearInDrive(
    const SsOperationInputs_t *
        in) // Helper function to check if the gear position is in Drive (D) based on the inputs
{
	return (in != NULL) && (in->gear_position == SS_GEAR_D);
}

static bool SS_StopBrakePressed(
    const SsOperationInputs_t *
        in) // Helper function to check if the brake pedal is pressed based on the inputs
{
	return (in != NULL) && (in->brake_pressed == true);
}

static bool SS_StopSpeedWithinLimit(
    const SsOperationInputs_t *
        in) // Helper function to check if the vehicle speed is within the limit for autostop based on the inputs
{
	return (in != NULL) && (in->vehicle_speed_kmh <= SS_AUTOSTOP_SPEED_MAX_KMH);
}

// Helper function to check if the conditions for engine stop are met based on the inputs
static bool SS_IsStopConditionMet(const SsOperationInputs_t *in) {
	return SS_StopIgnitionOn(in) && SS_StopEnabled(in) && SS_StopAllowed(in) &&
	       SS_StopGearInDrive(in) && SS_StopBrakePressed(in) &&
	       SS_StopSpeedWithinLimit(in);
}

static bool SS_RestartByAccelerator(
    const SsOperationInputs_t *
        in) // Helper function to check if the engine restart condition is met by accelerator pedal press based on the inputs
{
	return (in != NULL) && (in->accelerator_pressed == true);
}

static bool SS_RestartByBrakeRelease(
    const SsOperationInputs_t *
        in) // Helper function to check if the engine restart condition is met by brake pedal release based on the inputs
{
	return (in != NULL) && (in->brake_pressed == false);
}

static bool SS_RestartByGearChange(
    const SsOperationInputs_t *
        in) // Helper function to check if the engine restart condition is met by gear change from Drive (D) to another position based on the inputs
{
	return (in != NULL) && (in->gear_position != SS_GEAR_D);
}

static bool SS_RestartByDisable(
    const SsOperationInputs_t *
        in) //  Helper function to check if the engine restart condition is met by user disabling the start-stop system based on the inputs
{
	return (in != NULL) && (in->ss_enable == false);
}

static bool SS_RestartByTimeout(
    const SsOperationInputs_t *
        in) // Helper function to check if the engine restart condition is met by autostop timeout based on the inputs
{
	return (in != NULL) && (in->autostop_timeout_reached == true);
}

// Helper function to check if the conditions for engine restart are met based on the inputs
static bool SS_IsRestartConditionMet(const SsOperationInputs_t *in) {
	return SS_RestartByAccelerator(in) || SS_RestartByBrakeRelease(in) ||
	       SS_RestartByGearChange(in) || SS_RestartByDisable(in) ||
	       SS_RestartByTimeout(in);
}

// Initializes the start-stop system operation module by setting the initial state to VEHICLE_OFF
void SS_Operation_Init(void) { g_ss_op_state = SS_OP_STATE_VEHICLE_OFF; }

// Runs the start-stop system operation logic every 10ms based on the provided inputs and updates the outputs accordingly
void SS_Operation_Run10ms(const SsOperationInputs_t *inputs,
                          SsOperationOutputs_t *outputs, bool safe_stop) {
	if ((inputs != NULL) && (outputs != NULL)) {
		outputs->engine_stop_request = false; // Default to no stop request
		outputs->engine_restart_request =
		    false; // Default to no restart request
		outputs->autostop_active = false; //  Default to autostop not active
		outputs->state =
		    g_ss_op_state; // Update the output state to the current global state

		switch (g_ss_op_state) {
		// State machine logic to handle the transitions between VEHICLE_OFF, ENGINE_RUNNING, and AUTOSTOP_ACTIVE states based on the inputs and update the outputs accordingly
		case SS_OP_STATE_VEHICLE_OFF: {
			if (inputs->ignition_on == true) {
				g_ss_op_state = SS_OP_STATE_ENGINE_RUNNING;
			}
			break;
		}

		case SS_OP_STATE_ENGINE_RUNNING: {
			if (inputs->ignition_on == false) {
				g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
			} else if (SS_IsStopConditionMet(inputs) == true) {
				outputs->engine_stop_request = true;
				g_ss_op_state = SS_OP_STATE_AUTOSTOP_ACTIVE;
			} else {
				/* No state transition in this cycle. */
			}
			break;
		}

		case SS_OP_STATE_AUTOSTOP_ACTIVE: {
			outputs->autostop_active = true;

			if (inputs->ignition_on == false) {
				g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
			} else if ((SS_IsRestartConditionMet(inputs) == true) &&
			           (safe_stop == false)) {
				outputs->engine_restart_request = true;
				g_ss_op_state = SS_OP_STATE_ENGINE_RUNNING;
			} else {
				/* Remain in AUTOSTOP_ACTIVE state. */
			}
			break;
		}

		default: {
			g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
			break;
		}
		}

		outputs->state = g_ss_op_state;
		outputs->autostop_active =
		    (g_ss_op_state == SS_OP_STATE_AUTOSTOP_ACTIVE);
	}
}

SsOperationState_t SS_Operation_GetState(void) { return g_ss_op_state; }

bool SS_Operation_IsAutoStopActiveState(void) {
	return (g_ss_op_state == SS_OP_STATE_AUTOSTOP_ACTIVE);
}

// Test hooks and helper functions for unit testing - only included when UNIT_TEST is defined
#ifdef UNIT_TEST

/* --- Allow forcing internal state --- */
void SS_Operation_SetStateForTest(SsOperationState_t state) {
	g_ss_op_state = state;
}

/* --- Wrappers for static helpers --- */
bool SS_UT_StopEnabled(const SsOperationInputs_t *in) {
	return SS_StopEnabled(in);
}

bool SS_UT_StopAllowed(const SsOperationInputs_t *in) {
	return SS_StopAllowed(in);
}

bool SS_UT_StopGearInDrive(const SsOperationInputs_t *in) {
	return SS_StopGearInDrive(in);
}

bool SS_UT_StopBrakePressed(const SsOperationInputs_t *in) {
	return SS_StopBrakePressed(in);
}

bool SS_UT_StopSpeedWithinLimit(const SsOperationInputs_t *in) {
	return SS_StopSpeedWithinLimit(in);
}

bool SS_UT_RestartByAccelerator(const SsOperationInputs_t *in) {
	return SS_RestartByAccelerator(in);
}

bool SS_UT_RestartByBrakeRelease(const SsOperationInputs_t *in) {
	return SS_RestartByBrakeRelease(in);
}

bool SS_UT_RestartByGearChange(const SsOperationInputs_t *in) {
	return SS_RestartByGearChange(in);
}

bool SS_UT_RestartByDisable(const SsOperationInputs_t *in) {
	return SS_RestartByDisable(in);
}

bool SS_UT_RestartByTimeout(const SsOperationInputs_t *in) {
	return SS_RestartByTimeout(in);
}

bool SS_UT_IsStopConditionMet(const SsOperationInputs_t *in) {
	return SS_IsStopConditionMet(in);
}

bool SS_UT_StopIgnitionOn(const SsOperationInputs_t *in) {
	return SS_StopIgnitionOn(in);
}

#endif
