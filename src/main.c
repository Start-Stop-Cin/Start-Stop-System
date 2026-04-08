#include <stdio.h>
#include "ss_control.h"
#include "ss_inhibit_safety_conditions.h"
#include "ss_operation.h"
#include <time.h>

SsOperationInputs_t inputs_struct;
SsOperationOutputs_t outputs_struct;
SsOperationInputs_t *inputs = &inputs_struct;
SsOperationOutputs_t *outputs = &outputs_struct;

int main() {

	printf("Starting Start-Stop System Simulation...\n");

	// Initialize the start-stop system operation module
	SS_Operation_Init();

	// Mock input data
	inputs->accelerator_pressed = false;
	inputs->autostop_timeout_reached = true;
	inputs->brake_pressed = true;
	inputs->door_status = true; // All doors closed
	inputs->gear_position = SS_GEAR_N; // Park
	inputs->ignition_on = true;
	inputs->inclination_angle = 0.0f; // Flat surface
	inputs->seatbelt_status = true; // Seatbelt fastened
	inputs->ss_enable = true; // Start-Stop enabled by user
	inputs->vehicle_speed_kmh = 0.0f; // Stationary


    while (true)
    {
        /* UPDATE ALL INPUTS */

		//	Update the inhibit conditions based on the current inputs and outputs
        bool ss_inhibit_wrapper_retVal[2] = {false, false};
        ss_inhibit_wrapper(
            inputs->inclination_angle, inputs->door_status, inputs->seatbelt_status, inputs->gear_position, inputs->ss_enable,
            outputs->autostop_active, inputs->brake_pressed,
            inputs->ignition_on, outputs->engine_stop_request, inputs->vehicle_speed_kmh,
            ss_inhibit_wrapper_retVal 
        );
        inputs->autostop_allowed = ss_inhibit_wrapper_retVal[0];

        /* RUN OPERATION LOGIC */
        SS_Operation_Run10ms( inputs, outputs );

        print_inputs_outputs( inputs, outputs );
    }

    return 0;
}