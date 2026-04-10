# Start-Stop System – Software-in-the-Loop (SIL)

This folder contains the Software-in-the-Loop (SIL) environment for the Start-Stop system.

The SIL setup enables validation of the embedded C implementation integrated with a Simulink-based simulation environment and interactive dashboard inputs.

---

## Overview

The SIL model (`sil.slx`) integrates the full Start-Stop system software stack through a C interface, allowing real-time simulation and validation of system behavior.

The system is executed through a periodic function:

```c
SS_App_Run10ms_If(...)
```

which represents the main application cycle, executed every 10 ms.

---

## Architecture

The SIL environment is structured as follows:

```text
Dashboard Inputs
        ↓
Signal Conditioning (Simulink)
        ↓
SS_App_Run10ms_If (C Interface)
        ↓
----------------------------------------
|   ss_control          (Enable logic)  |
|   ss_input_proc       (Input handling)|
|   ss_inhibit          (Safety logic)  |
|   ss_operation        (State machine) |
|   ss_timer            (Timing logic)  |
|   ss_co2              (CO2 estimation)|
|   ss_fuel             (Fuel estimation)|
----------------------------------------
        ↓
Outputs → Dashboard / Displays
```

---

## SIL Model (`sil.slx`)

The model consists of:

* **Dashboard Controls**

  * Ignition switch
  * Brake / accelerator sliders
  * Gear selector
  * Vehicle speed
  * Inclination angle
  * Door and seatbelt status
  * Start-Stop button (pulse)

* **C Function Block**

  * Calls `SS_App_Run10ms_If`
  * Executes full system logic

* **Outputs Visualization**

  * State machine state
  * AutoStopActive
  * EngineStopRequest / RestartRequest
  * SafeStop
  * AutoStopAllowed
  * CO₂ avoided
  * Fuel saved
  * HMI signals

---

## Execution Model

* Simulation type: **Fixed-step**
* Step size: **0.01 s (10 ms)**
* Execution model: **Discrete**

Each simulation step represents one execution of the embedded software cycle.

---

## Integration with C Code

The SIL model interfaces with the C implementation through:

```c
SS_App_Run10ms_If(...)
```

### Responsibilities of the application layer:

* Receive raw inputs from Simulink
* Process signals (input processing)
* Execute system logic
* Update timers and estimations
* Return outputs to the model

---

## Inputs Mapping

The following signals are provided by the dashboard:

| Signal                         | Description               |
| ------------------------------ | ------------------------- |
| ignition_on                    | Vehicle ignition status   |
| brake_pedal_position_pct       | Brake pedal position      |
| accelerator_pedal_position_pct | Accelerator position      |
| gear_position                  | Current gear (P, R, N, D) |
| vehicle_speed_kmh              | Vehicle speed             |
| door_status                    | Door closed/open          |
| seatbelt_status                | Seatbelt status           |
| button_input                   | Start-Stop button         |
| inclination_angle_rad          | Road inclination          |

---

## Outputs Mapping

| Signal                   | Description               |
| ------------------------ | ------------------------- |
| ss_enable                | Start-Stop enabled state  |
| hmi_led                  | HMI indicator             |
| autostop_allowed         | AutoStop availability     |
| safe_stop                | SafeStop condition        |
| autostop_active          | AutoStop active state     |
| engine_stop_request      | Stop request              |
| engine_restart_request   | Restart request           |
| state                    | State machine state       |
| autostop_event_time_ms   | Current AutoStop duration |
| autostop_timeout_reached | Timeout flag              |
| co2_avoided_g            | CO₂ avoided               |
| fuel_saved_l             | Fuel saved                |
| display_co2              | CO₂ display flag          |
| display_fuel             | Fuel display flag         |

---

## Key Features Validated in SIL

The SIL environment allows validation of:

* Start-Stop state machine transitions
* AutoStop and AutoStart conditions
* Safety and inhibit logic
* SafeStop behavior
* Drive cycle memory logic
* Timer-based restart
* CO₂ avoided estimation
* Fuel saving estimation
* HMI behavior and signals

---

## How to Run

1. Open MATLAB
2. Add project folder to path:

```matlab
addpath(genpath('path_to_project'))
```

3. Open SIL model:

```matlab
open('sil.slx')
```

4. Launch the dashboard:

* Click **Start**
* Interact with controls

---

## Usage Notes

* The system requires a valid drive cycle (speed > 8 km/h) before enabling AutoStop.
* SafeStop may prevent engine restart under unsafe conditions.
* CO₂ and fuel estimations accumulate across AutoStop events.
* All accumulated values reset when ignition is turned OFF.

---

## Purpose

The SIL environment is used to:

* Validate system behavior before deployment
* Demonstrate functionality interactively
* Verify integration between modules
* Support development and debugging

---

## Limitations

* Vehicle dynamics are simplified
* Fuel and CO₂ estimations are based on calibrated assumptions
* Not intended for production-level accuracy

---

## Dashboard Test Scenarios

The following scenarios can be executed using the Simulink dashboard to validate system behavior.

---

### 1. Basic AutoStop Activation

**Initial Conditions:**

* Ignition: ON
* Gear: D
* Vehicle Speed: 0 km/h  (The driver shall set Vehicle Speed > 8 in the currente drive cycle to ensure AutoStopAllowed = TRUE)
* Brake Pressed: YES (>5%)
* Accelerator: 0%
* Door: Closed
* Seatbelt: Fastened
* SS Enabled: TRUE

**Action:**

* Maintain brake pressed at standstill

**Expected Result:**

* `AutoStopActive = TRUE`
* `EngineStopRequest = TRUE`
* Engine transitions to AutoStop state
* CO₂ and Fuel counters start increasing

---

### 2. Inhibit by Inclination

**Initial Conditions:**

* Same as Scenario 1

**Action:**

* Set Inclination Angle > 0.15

**Expected Result:**

* `AutoStopAllowed = FALSE`
* `AutoStopActive = FALSE`
* Engine remains running

---

### 3. Drive Cycle Memory Behavior

**Initial Conditions:**

* Ignition: ON
* Vehicle Speed: 0 km/h

**Action:**

1. Attempt AutoStop without prior driving
2. Increase speed > 8 km/h
3. Return to 0 km/h and attempt AutoStop again

**Expected Result:**

* First attempt: `AutoStopAllowed = FALSE`
* After driving: `AutoStopAllowed = TRUE`
* System now allows AutoStop

---

### 4. Restart by Time (Timeout)

**Initial Conditions:**

* Vehicle in AutoStop

**Action:**

* Keep vehicle stopped for extended time (3 MINUTES)

**Expected Result:**

* `AutoStopTimeoutReached = TRUE`
* `EngineRestartRequest = TRUE`
* Engine returns to EngineRunning state

---

### 5. Restart by Driver Input

**Initial Conditions:**

* Vehicle in AutoStop

**Action (any of the following):**

* Release brake
* Press accelerator
* Change gear

**Expected Result:**

* `EngineRestartRequest = TRUE`
* `AutoStopActive = FALSE`
* Engine restarts immediately

---

### 6. Safe Stop Activation (Door or Seatbelt)

**Initial Conditions:**

* Vehicle in AutoStop

**Action:**

* Open door OR unfasten seatbelt

**Expected Result:**

* `SafeStop = TRUE`
* Engine restart is **blocked**
* Vehicle remains in AutoStop until safe conditions are restored

---

### 7. Safe Stop Release

**Initial Conditions:**

* SafeStop active

**Action:**

* Close door AND fasten seatbelt
* Press brake

**Expected Result:**

* `SafeStop = FALSE`
* Restart conditions can now be processed
* Engine restarts if conditions are met

---

### 8. Button Enable/Disable

**Initial Conditions:**

* Ignition ON

**Action:**

* Press SS button (Pulse)

**Expected Result:**

* `SS_Enable` toggles
* `HMI_LED` updates accordingly
* If disabled:

  * `AutoStopAllowed = FALSE`
  * No AutoStop events occur

---

### 9. CO₂ and Fuel Accumulation

**Initial Conditions:**

* Enter AutoStop

**Action:**

* Keep vehicle in AutoStop

**Expected Result:**

* `CO2_avoided_g` increases over time
* `fuel_saved_l` increases over time

**After exiting AutoStop:**

* Values remain stored
* Reset only when Ignition = OFF

---

### 10. Ignition Reset Behavior

**Initial Conditions:**

* Accumulated CO₂ and Fuel values

**Action:**

* Turn Ignition OFF

**Expected Result:**

* All accumulated values reset:

  * `CO2_avoided_g = 0`
  * `fuel_saved_l = 0`
  * timers reset

---

## Notes

* All scenarios are executed using the dashboard interface in `sil.slx`.
* The system runs with a fixed-step simulation of 10 ms.
* All logic is executed through the `SS_App_Run10ms_If` interface.