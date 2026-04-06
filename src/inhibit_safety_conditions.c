#include "../include/inhibit_safety_conditions.h"
#include <stdio.h>

int inhibit_safety_conditions( float InclinationAngle, int DoorStatus, int SeatbeltStatus, int GearPosition, int SS_Enabled ) {
    if (InclinationAngle > 0.15f && GearPosition == 2 && DoorStatus == 1 && SeatbeltStatus == 1 && SS_Enabled == 1) {
        return 1;
    }
    return 0;
}

int standstill_management( int AutoStopActive, int BrakeStatus, int DoorStatus, int SeatbeltStatus ) {
    
    static int AutoStopActive_d = 0;
    static int Q_d = 0;

    int setValue = AutoStopActive && ( !DoorStatus || !SeatbeltStatus );
    int resetValue = BrakeStatus && DoorStatus && SeatbeltStatus;

    int Q = sr_flip_flop(setValue, resetValue);
    int SafeStop = Q_d;

    AutoStopActive_d = AutoStopActive;
    Q_d = Q;

    return SafeStop;
}

int drivecycle_memory_block ( int IgnitionStatus, int EngineStopRequest, float VehicleSpeed ) {
    static int y_1 = 0;

    int or1 = ((IgnitionStatus == 0) || EngineStopRequest);
    int or2 = ((VehicleSpeed > 8) || y_1);
    int y = or1 ? 0 : or2;

    y_1 = y;

    return y;
}

int sr_flip_flop(int S, int R, sr_state_t * state) {

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

sr_state_t flipFlopState = {0}; // Initial state of the flip-flop

int * inhibit_wrapper(  float InclinationAngle, int DoorStatus, int SeatbeltStatus, int GearPosition, int SS_Enabled,
                        int AutoStopActive, int BrakeStatus,
                        int IgnitionStatus, int EngineStopRequest, float VehicleSpeed,
                        int * retVal 
                    ) {

    retVal[0] = inhibit_safety_conditions(InclinationAngle, DoorStatus, SeatbeltStatus, GearPosition, SS_Enabled) && standstill_management(AutoStopActive, BrakeStatus, DoorStatus, SeatbeltStatus);
    retVal[1] = drivecycle_memory_block(IgnitionStatus, EngineStopRequest, VehicleSpeed);

    // // For debug purpose
    // printf("AutoStopAllowed: %i\n", retVal[0]);
    // printf("SafeStop: %i\n", retVal[1]);

    return retVal;
}