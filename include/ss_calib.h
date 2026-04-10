#ifndef SS_CALIB_H
#define SS_CALIB_H

#define SS_TASK_PERIOD_MS                    (10U) // Timer task period in milliseconds (10ms)

#define SS_AUTOSTOP_SPEED_MAX_KMH            (5.0f) // Maximum vehicle speed in km/h for autostop to be allowed (e.g., 5 km/h)
#define SS_DRIVECYCLE_SPEED_THRESHOLD_KMH    (8.0f) // Speed threshold in km/h to consider the vehicle as moving for drive cycle detection (e.g., 8 km/h)

#define SS_AUTOSTOP_TIMEOUT_MS               (180000U) // Autostop timeout in milliseconds (e.g., 180000ms = 3 minutes) - time after which the system will automatically restart the engine if autostop is active
#define SS_BUTTON_DEBOUNCE_MS                (50U) // Button debounce time in milliseconds (e.g., 50ms) - time to ignore rapid changes in button states to prevent false triggers

#define SS_CO2_WLTP_IDLE_FACTOR               (10.0f)

#define SS_BRAKE_PRESSED_THRESHOLD_PCT       (5.0f)
#define SS_ACCEL_PRESSED_THRESHOLD_PCT       (5.0f)

#define SS_INCLINATION_MAX_RAD               (0.15f)

#endif