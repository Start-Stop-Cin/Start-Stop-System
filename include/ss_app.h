#ifndef SS_APP_H
#define SS_APP_H

#include <stdbool.h>

void SS_App_Init(void);

void SS_App_Run10ms_If(
    bool ignition_on,
    bool ss_enable,
    float brake_pedal_position_pct,
    float accelerator_pedal_position_pct,
    int gear_position,
    float vehicle_speed_kmh,
    bool door_status,
    bool seatbelt_status,
    bool button_input,
    float inclination_angle_rad,
    bool *autostop_allowed,
    bool *safe_stop,
    bool *autostop_active,
    bool *engine_stop_request,
    bool *engine_restart_request,
    int *state,
    unsigned int *autostop_event_time_ms,
    bool *autostop_timeout_reached);

#endif