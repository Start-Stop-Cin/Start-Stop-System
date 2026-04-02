#include "../include/inhibit_safety_conditions.h"

int inhibit_safety_conditions( float InclinationAngle, int DoorStatus, int SeatbeltStatus, int GearPosition, int SS_Enabled ) {
    if (InclinationAngle > 0.15f && GearPosition == 2 && DoorStatus == 1 && SeatbeltStatus == 1 && SS_Enabled == 1) {
        return 1;
    }
    return 0;
}