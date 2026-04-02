#ifndef INHIBIT_SAFETY_CONDITIONS_H
#define INHIBIT_SAFETY_CONDITIONS_H

int inhibit_safety_conditions( float InclinationAngle, int DoorStatus, int SeatbeltStatus, int GearPosition, int SS_Enabled );

int standstill_management( int AutoStopActive, int BrakeStatus, int DoorStatus, int SeatbeltStatus );

int sr_flip_flop(int S, int R);

#endif