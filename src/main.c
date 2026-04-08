#include "ss_inhibit_safety_conditions.h"
#include "ss_operation.h"

SsOperationInputs_t *inputs;
SsOperationOutputs_t *outputs;

int main () {

    while (true)
    {

        bool * ss_inhibit_wrapper_retVal;
        ss_inhibit_wrapper(
            inputs->inclination_angle, inputs->door_status, inputs->seatbelt_status, inputs->gear_position, inputs->ss_enable,
            outputs->autostop_active, inputs->brake_pressed,
            inputs->ignition_on, outputs->engine_stop_request, inputs->vehicle_speed_kmh,
            ss_inhibit_wrapper_retVal 
        );
        inputs->autostop_allowed = ss_inhibit_wrapper_retVal[0];

        SS_Operation_Run10ms( inputs, outputs );
    }
    

    return 0;
}