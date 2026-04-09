#ifndef SS_CO2_H
#define SS_CO2_H

#include <stdbool.h>
#include <stdint.h>

// This header file defines the interface for the CO2 calculation module of the
// start-stop system, which tracks the accumulated autostop time, calculates the
// avoided CO2 emissions based on a calibration factor, and provides functions
// to retrieve this information for display purposes in the user interface.

void Co2_Init(void); // Initializes the CO2 module by resetting all relevant
    // variables to their default states
void Co2_Run10ms(
    bool ignition_on,
    bool autostop_active); //  Runs the CO2 calculation logic every 10ms to
//  update the accumulated autostop time, calculate
//  the avoided CO2 grams, and set display/hide flags
//  based on ignition and autostop status

uint32_t Co2_GetAccumulatedAutoStopTimeMs(
    void); // Retrieves the total accumulated autostop time in milliseconds,
// which can be used to calculate the total CO2 avoided or for
// display purposes in the user interface
float Co2_GetAvoidedGrams(
    void); //  Retrieves the total amount of CO2 avoided in grams based on the
//  accumulated autostop time and a calibration factor, which can be
//  used for display purposes or to provide feedback to the driver
//  about their environmental impact

bool Co2_DisplayEnabled(
    void); //  Checks if the CO2 information should be displayed, which is
//  typically true when autostop is active, allowing the user
//  interface to show the avoided CO2 grams to the driver
bool Co2_HideEnabled(
    void); //  Checks if the CO2 information should be hidden, which is
//  typically true when autostop is not active, allowing the user
//  interface to hide the avoided CO2 grams when the vehicle is not
//  in autostop mode

#endif
