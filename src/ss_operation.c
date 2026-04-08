#include "ss_operation.h"
#include "ss_calib.h"
#include <stdio.h>


// Global variable to hold the current state of the start-stop system operation - initialized to VEHICLE_OFF
static SsOperationState_t g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;


static bool SS_StopIgnitionOn(const SsOperationInputs_t *in) // Helper function to check if the ignition is on based on the inputs
{
    return (in != 0) && (in->ignition_on == true);
}

static bool SS_StopEnabled(const SsOperationInputs_t *in) //  Helper function to check if the start-stop system is enabled by the user based on the inputs
{
    return (in != 0) && (in->ss_enable == true);
}

static bool SS_StopAllowed(const SsOperationInputs_t *in) // Helper function to check if the current conditions allow for autostop based on the inputs
{
    return (in != 0) && (in->autostop_allowed == true);
}

static bool SS_StopGearInDrive(const SsOperationInputs_t *in) // Helper function to check if the gear position is in Drive (D) based on the inputs
{
    return (in != 0) && (in->gear_position == SS_GEAR_D);
}

static bool SS_StopBrakePressed(const SsOperationInputs_t *in) // Helper function to check if the brake pedal is pressed based on the inputs
{
    return (in != 0) && (in->brake_pressed == true);
}

static bool SS_StopSpeedWithinLimit(const SsOperationInputs_t *in) // Helper function to check if the vehicle speed is within the limit for autostop based on the inputs
{
    return (in != 0) && (in->vehicle_speed_kmh <= SS_AUTOSTOP_SPEED_MAX_KMH);
}

// Helper function to check if the conditions for engine stop are met based on the inputs
static bool SS_IsStopConditionMet(const SsOperationInputs_t *in)
{
    return SS_StopIgnitionOn(in) &&
           SS_StopEnabled(in) &&
           SS_StopAllowed(in) &&
           SS_StopGearInDrive(in) &&
           SS_StopBrakePressed(in) &&
           SS_StopSpeedWithinLimit(in);
}


static bool SS_RestartByAccelerator(const SsOperationInputs_t *in) // Helper function to check if the engine restart condition is met by accelerator pedal press based on the inputs
{
    return (in != 0) && (in->accelerator_pressed == true);
}

static bool SS_RestartByBrakeRelease(const SsOperationInputs_t *in) // Helper function to check if the engine restart condition is met by brake pedal release based on the inputs
{
    return (in != 0) && (in->brake_pressed == false);
}

static bool SS_RestartByGearChange(const SsOperationInputs_t *in) // Helper function to check if the engine restart condition is met by gear change from Drive (D) to another position based on the inputs
{
    return (in != 0) && (in->gear_position != SS_GEAR_D);
}

static bool SS_RestartByDisable(const SsOperationInputs_t *in) //  Helper function to check if the engine restart condition is met by user disabling the start-stop system based on the inputs
{
    return (in != 0) && (in->ss_enable == false);
}

static bool SS_RestartByTimeout(const SsOperationInputs_t *in)  // Helper function to check if the engine restart condition is met by autostop timeout based on the inputs
{
    return (in != 0) && (in->autostop_timeout_reached == true);
}

// Helper function to check if the conditions for engine restart are met based on the inputs
static bool SS_IsRestartConditionMet(const SsOperationInputs_t *in)
{
    return SS_RestartByAccelerator(in) ||
           SS_RestartByBrakeRelease(in) ||
           SS_RestartByGearChange(in) ||
           SS_RestartByDisable(in) ||
           SS_RestartByTimeout(in);
}

// Initializes the start-stop system operation module by setting the initial state to VEHICLE_OFF
void SS_Operation_Init(void)
{
    g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
}


// Runs the start-stop system operation logic every 10ms based on the provided inputs and updates the outputs accordingly
void SS_Operation_Run10ms(const SsOperationInputs_t *inputs, SsOperationOutputs_t *outputs)
{
    if ((inputs == 0) || (outputs == 0))
    {
        return;
    }

    outputs->engine_stop_request = false; // Default to no stop request
    outputs->engine_restart_request = false;  // Default to no restart request
    outputs->autostop_active = false; //  Default to autostop not active
    outputs->state = g_ss_op_state;  // Update the output state to the current global state

    switch (g_ss_op_state)
    {
        // State machine logic to handle the transitions between VEHICLE_OFF, ENGINE_RUNNING, and AUTOSTOP_ACTIVE states based on the inputs and update the outputs accordingly
        case SS_OP_STATE_VEHICLE_OFF:
        {
            if (inputs->ignition_on == true)
            {
                g_ss_op_state = SS_OP_STATE_ENGINE_RUNNING;
            }
            break;
        }

        case SS_OP_STATE_ENGINE_RUNNING:
        {
            if (inputs->ignition_on == false)
            {
                g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
            }
            else if (SS_IsStopConditionMet(inputs) == true)
            {
                outputs->engine_stop_request = true;
                g_ss_op_state = SS_OP_STATE_AUTOSTOP_ACTIVE;
            }
            break;
        }

        case SS_OP_STATE_AUTOSTOP_ACTIVE:
        {
            outputs->autostop_active = true;

            if (inputs->ignition_on == false)
            {
                g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
            }
            else if (SS_IsRestartConditionMet(inputs) == true)
            {
                outputs->engine_restart_request = true;
                g_ss_op_state = SS_OP_STATE_ENGINE_RUNNING;
            }
            break;
        }

        default:
        {
            g_ss_op_state = SS_OP_STATE_VEHICLE_OFF;
            break;
        }
    }

    outputs->state = g_ss_op_state;
    outputs->autostop_active = (g_ss_op_state == SS_OP_STATE_AUTOSTOP_ACTIVE);
}

void print_inputs_outputs ( const SsOperationInputs_t *inputs, SsOperationOutputs_t *outputs ) {
    printf("Inputs:\n");
    printf("Ignition On: %d\n", inputs->ignition_on);
    printf("SS Enable: %d\n", inputs->ss_enable);
    printf("Autostop Allowed: %d\n", inputs->autostop_allowed);
    printf("Door Status: %d\n", inputs->door_status);
    printf("Seatbelt Status: %d\n", inputs->seatbelt_status);
    printf("Brake Pressed: %d\n", inputs->brake_pressed);
    printf("Accelerator Pressed: %d\n", inputs->accelerator_pressed);
    printf("Gear Position: %d\n", inputs->gear_position);
    printf("Vehicle Speed (km/h): %.2f\n", inputs->vehicle_speed_kmh);
    printf("Inclination Angle: %.2f\n", inputs->inclination_angle);
    printf("Autostop Timeout Reached: %d\n", inputs->autostop_timeout_reached);
    printf("Button Input: %d\n", inputs->ButtonInput);

    printf("\nOutputs:\n");
    printf("Engine Stop Request: %d\n", outputs->engine_stop_request);
    printf("Engine Restart Request: %d\n", outputs->engine_restart_request);
    printf("Autostop Active: %d\n", outputs->autostop_active);
    printf("State: %d\n", outputs->state);
}