#include "ss_timer.h"
#include "ss_calib.h"

static uint32_t g_autostop_event_time_ms =
    0U; // Global variable to track the time in milliseconds since the autostop event started, which is used to determine if the autostop timeout has been reached based on a calibration value defined in ss_calib.h
static bool g_autostop_timeout_reached =
    false; // Global variable to indicate whether the autostop timeout has been reached, which is set to true when the accumulated autostop event time exceeds the autostop timeout defined in ss_calib.h, allowing the system to automatically restart the engine if autostop is active for too long

void SS_Timer_Init(void) {
	g_autostop_event_time_ms =
	    0U; //  Initialize the autostop event time to 0 milliseconds at the start of the system, ensuring that the timer starts from a known state when the vehicle is turned on
	g_autostop_timeout_reached =
	    false; //  Initialize the autostop timeout flag to false at the start of the system, indicating that the autostop timeout has not been reached when the vehicle is first turned on
}

// Runs the timer logic every 10ms to update the accumulated autostop event time and determine if the autostop timeout has been reached based on whether autostop is currently active.
// If autostop is active, the accumulated time is incremented by the task period (10ms) each time this function is called, and the timeout flag is set to true if the accumulated time
// exceeds the autostop timeout defined in ss_calib.h. If autostop is not active, the accumulated time is reset to 0 and the timeout flag is set to false, ensuring that the timer only
// counts up when autostop is active and resets when it is not.
void SS_Timer_Run10ms(bool autostop_active) {
	if (autostop_active == true) {
		if (g_autostop_event_time_ms <
		    SS_AUTOSTOP_TIMEOUT_MS) // SS_AUTOSTOP_TIMEOUT_MS is defined in ss_calib.h and represents the autostop timeout in milliseconds (e.g., 180000ms = 3 minutes)
		{
			g_autostop_event_time_ms += SS_TASK_PERIOD_MS;
		}

		g_autostop_timeout_reached =
		    (g_autostop_event_time_ms >= SS_AUTOSTOP_TIMEOUT_MS);
	} else {
		g_autostop_event_time_ms = 0U;
		g_autostop_timeout_reached = false;
	}
}

// Retrieves if the autostop timeout has been reached
bool SS_Timer_IsAutostopTimeoutReached(void) {
	return g_autostop_timeout_reached;
}

// Retrieves the current accumulated autostop event time in milliseconds
uint32_t SS_Timer_GetAutostopEventTimeMs(void) {
	return g_autostop_event_time_ms;
}
