#include <stdio.h>
#include "ss_control.h"
#include "ss_inhibit_safety_conditions.h"
#include "ss_operation.h"
#include <time.h>

SsOperationInputs_t *inputs;
SsOperationOutputs_t *outputs;

int main() {

    while (true)
    {
        // Update all inputs

        bool * ss_inhibit_wrapper_retVal = {0};
        ss_inhibit_wrapper(
            inputs->inclination_angle, inputs->door_status, inputs->seatbelt_status, inputs->gear_position, inputs->ss_enable,
            outputs->autostop_active, inputs->brake_pressed,
            inputs->ignition_on, outputs->engine_stop_request, inputs->vehicle_speed_kmh,
            ss_inhibit_wrapper_retVal 
        );
        inputs->autostop_allowed = ss_inhibit_wrapper_retVal[0];


        //  Run the operation logic every 10ms with all updated inputs
        SS_Operation_Run10ms( inputs, outputs );

        print_inputs_outputs( inputs, outputs );
    }

    return 0;
}