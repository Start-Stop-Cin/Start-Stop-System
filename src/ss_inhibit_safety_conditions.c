#include "ss_inhibit_safety_conditions.h"
#include <stdio.h>
#include <stdbool.h>

// Checks if all safety conditions are met to allow autostop based on the inclination angle, door status, seatbelt status, gear position, and whether the start-stop system is enabled.
bool inhibit_safety_conditions( float InclinationAngle, bool DoorStatus, bool SeatbeltStatus, bool GearPosition, bool SS_Enabled ) {
    if (InclinationAngle > 0.15f && GearPosition == 2 && DoorStatus == true && SeatbeltStatus == true && SS_Enabled == true) {
        return true;
    }
    return 0;
}

sr_state_t flipFlopState = {0}; // Initial state of the flip-flop

//  Check if safe-stop is reachable
bool standstill_management( bool AutoStopActive, bool BrakeStatus, bool DoorStatus, bool SeatbeltStatus ) {
    
    static bool AutoStopActive_d = 0;
    static bool Q_d = 0;

    bool setValue = AutoStopActive && ( !DoorStatus || !SeatbeltStatus );
    bool resetValue = BrakeStatus && DoorStatus && SeatbeltStatus;

    bool Q = sr_flip_flop(setValue, resetValue, &flipFlopState);
    bool SafeStop = Q_d;

    AutoStopActive_d = AutoStopActive;
    Q_d = Q;

    return SafeStop;
}

bool drivecycle_memory_block ( bool IgnitionStatus, bool EngineStopRequest, float VehicleSpeed ) {
    static bool y_1 = 0;

    bool or1 = ((IgnitionStatus == 0) || EngineStopRequest);
    bool or2 = ((VehicleSpeed > 8) || y_1);
    bool y = or1 ? 0 : or2;

    y_1 = y;

    return y;
}

bool sr_flip_flop(bool S, bool R, sr_state_t * state) {

    /*
        I have leaved this module state as a parameter to be able to test it in isolation,
        thus improving the testability of the code.
    */

    if (S == 1 && R == 0) {
        state->Q = 1;
    } else if (S == 0 && R == 1) {
        state->Q = 0;
    } else if (S == 1 && R == 1) {
        state->Q = -1;
    }

    return state->Q;
}

bool * ss_inhibit_wrapper(  float InclinationAngle, bool DoorStatus, bool SeatbeltStatus, bool GearPosition, bool SS_Enabled,
                        bool AutoStopActive, bool BrakeStatus,
                        bool IgnitionStatus, bool EngineStopRequest, float VehicleSpeed,
                        bool * retVal 
                    ) {

    retVal[0] = inhibit_safety_conditions(InclinationAngle, DoorStatus, SeatbeltStatus, GearPosition, SS_Enabled) && drivecycle_memory_block(IgnitionStatus, EngineStopRequest, VehicleSpeed);
    retVal[1] = standstill_management(AutoStopActive, BrakeStatus, DoorStatus, SeatbeltStatus);

    return retVal;
}

int test_inhibit_wrapper(void) {
    // Manual test for the wrapper function

    bool retVal[2] = {0};

    ss_inhibit_wrapper(0.2f, true, true, true, true, true, true, true, true, 0.0f, retVal);

    printf("AutoStopAllowed: %d\n", retVal[0]);
    printf("SafeStop: %d\n", retVal[1]);

    return 0;
}

