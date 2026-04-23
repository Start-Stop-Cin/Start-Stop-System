#ifndef SS_APP_H
#define SS_APP_H

#include <stdbool.h>

void SS_App_Init(void);

void SS_App_Run10ms_If(
    bool ignition_on, float brake_pedal_position_pct,
    float accelerator_pedal_position_pct, int gear_position,
    float vehicle_speed_kmh, bool door_status, bool seatbelt_status,
    bool button_input, float inclination_angle_rad, bool *ss_enable,
    bool *hmi_led, bool *led_update_cmd, bool *button_rise_valid,
    bool *autostop_allowed, bool *safe_stop, bool *autostop_active,
    bool *engine_stop_request, bool *engine_restart_request, int *state,
    unsigned int *autostop_event_time_ms, bool *autostop_timeout_reached,
    unsigned int *autostop_accumulated_time_ms, float *co2_avoided_g,
    bool *display_co2, bool *hide_co2,
    unsigned int *autostop_accumulated_time_ms_fuel, float *fuel_saved_l,
    bool *display_fuel, bool *hide_fuel);

#endif
