#include "co2.h"
#include "ss_calib.h"

static uint32_t g_autostop_accumulated_time_ms =
    0U; //  Global variable to track the accumulated autostop time in
//  milliseconds
static float g_co2_avoided_g =
    0.0f; // Global variable to track the amount of CO2 avoided in grams based
// on the accumulated autostop time and a calibration factor
static bool g_display_co2 =
    false; // Global variable to indicate whether the CO2 information should be
// displayed, which is typically true when autostop is active
static bool g_hide_co2 =
    true; // Global variable to indicate whether the CO2 information should be
// hidden, which is typically true when autostop is not active

// Initializes the CO2 module for the start-stop system by resetting the
// accumulated autostop time, avoided CO2 grams, and display/hide flags to their
// default states
void Co2_Init(void) {
	g_autostop_accumulated_time_ms = 0U;
	g_co2_avoided_g = 0.0f;
	g_display_co2 = false;
	g_hide_co2 = true;
}

// Runs the CO2 calculation logic every 10ms to update the accumulated autostop
// time, calculate the avoided CO2 grams based on the accumulated time and a
// calibration factor, and set the display/hide flags based on whether the
// ignition is on and autostop is active
void Co2_Run10ms(bool ignition_on, bool autostop_active) {
	// If the ignition is off, reset all CO2-related variables to their default
	// states since the vehicle is not in use
	if (ignition_on == false) {
		g_autostop_accumulated_time_ms = 0U;
		g_co2_avoided_g = 0.0f;
		g_display_co2 = false;
		g_hide_co2 = true;
	}

	// If the ignition is on, update the accumulated autostop time and calculate
	// the avoided CO2 grams based on the accumulated time and a calibration
	// factor. Also, set the display/hide flags based on whether autostop is
	// currently active.
	else {
		if (autostop_active == true) {
			g_autostop_accumulated_time_ms +=
			    SS_TASK_PERIOD_MS; // Increment the accumulated autostop time by
			// the task period (10ms) each time this
			// function is called while autostop is active
		}

		g_co2_avoided_g =
		    ((float)g_autostop_accumulated_time_ms / 1000.0f) *
		    SS_CO2_WLTP_IDLE_FACTOR; // Convert accumulated time from milliseconds
		// to seconds and multiply by the WLTP idle
		// factor to get the avoided CO2 in grams

		g_display_co2 =
		    autostop_active; // Display CO2 information when autostop is active
		g_hide_co2 =
		    (autostop_active ==
		     false); // Hide CO2 information when autostop is not active
	}
}

// Retrieves the total accumulated autostop time in milliseconds, which can be
// used to calculate the total CO2 avoided or for display purposes in the user
// interface
uint32_t Co2_GetAccumulatedAutoStopTimeMs(void) {
	return g_autostop_accumulated_time_ms;
}

//  Retrieves the total amount of CO2 avoided in grams based on the accumulated
//  autostop time and a calibration factor, which can be used for display
//  purposes or to provide feedback to the driver about their environmental
//  impact
float Co2_GetAvoidedGrams(void) { return g_co2_avoided_g; }

//  Checks if the CO2 information should be displayed, which is typically true
//  when autostop is active, allowing the user interface to show the avoided CO2
//  grams to the driver
bool Co2_DisplayEnabled(void) { return g_display_co2; }

// Checks if the CO2 information should be hidden, which is typically true when
// autostop is not active, allowing the user interface to hide the avoided CO2
// grams when the vehicle is not in autostop mode
bool Co2_HideEnabled(void) { return g_hide_co2; }
