#include "ss_fuel.h"
#include "ss_calib.h"

static uint32_t g_autostop_accumulated_time_ms_fuel = 0U;
static float g_fuel_saved_l = 0.0f;
static bool g_display_fuel = false;
static bool g_hide_fuel = true;

void SS_Fuel_Init(void) {
	g_autostop_accumulated_time_ms_fuel = 0U;
	g_fuel_saved_l = 0.0f;
	g_display_fuel = false;
	g_hide_fuel = true;
}

void SS_Fuel_Run10ms(bool ignition_on, bool autostop_active) {
	if (ignition_on == false) {
		g_autostop_accumulated_time_ms_fuel = 0U;
		g_fuel_saved_l = 0.0f;
		g_display_fuel = false;
		g_hide_fuel = true;
	} else {
		if (autostop_active == true) {
			g_autostop_accumulated_time_ms_fuel += SS_TASK_PERIOD_MS;
		}

		g_fuel_saved_l =
		    ((float)g_autostop_accumulated_time_ms_fuel / 1000.0f) *
		    SS_FUEL_IDLE_RATE_LPS;

		g_display_fuel = autostop_active;
		g_hide_fuel = (autostop_active == false);
	}
}

uint32_t SS_Fuel_GetAccumulatedAutoStopTimeMs(void) {
	return g_autostop_accumulated_time_ms_fuel;
}

float SS_Fuel_GetSavedLiters(void) { return g_fuel_saved_l; }

bool SS_Fuel_DisplayEnabled(void) { return g_display_fuel; }

bool SS_Fuel_HideEnabled(void) { return g_hide_fuel; }
