# Start-Stop System: Software-in-the-Loop (SIL) Validation Environment

## Getting Started

1. Open the main testing model: `tests_model.slx`.
2. In the main canvas of the Simulink model, you will find a dedicated UI panel with an interactive button to launch the testing routine.
3. Open the ClusterSIL application to visualize the testing routine on a complete vehicle environment simulation.

Windows: `Start-Stop-System/src/Cluster_SIL/release_build/ClusterSIL.exe`
Linux: `Start-Stop-System/src/Cluster_SIL/linux_build` and use the command `./ClusterSIL-x86_64.AppImage --appimage-extract-and-run`

## Automated Validation Suite

The Automatic Simulation mode executes a sequentially injects 19 predefined test cases (LL-01 to LL-19) to validate the core logic, debounce filters, safe-stop routines, and memory resets without human intervention.

[Detailed Tests Spreadsheet](https://docs.google.com/spreadsheets/d/1PdqRRzYBHCJ0tb6e9vSiisdIVNVEFBRR/edit?usp=sharing&ouid=103339627942635382838&rtpof=true&sd=true)

### How to Execute:
1. In the `tests_model.slx` canvas, locate the **AUTOMATIC SIMULATION** section.
2. Click the **Execute Automated Tests** button. 
   *(Alternatively, run the `run_start_stop_tests.m` script directly from the MATLAB Editor).*

### Observation & Feedback:
* **Interactive Monitoring:** Launch the automated testing routine to interactively observe the Stateflow logic and Cluster operation during the simulation. The dashboard will automatically reflect the injected vehicle dynamics (speed, RPM, gear).
* **Live Console Tracking:** While the Simulink model runs in the background, the MATLAB Command Window acts as a live feed transmission, printing the exact timestamp and description of the test currently under execution:
  ```text
  [T = 000.0 s] >> Executing [LL-01]: AutoStop Entry (Drive Cycle)
  [T = 010.1 s] >> Executing [LL-02]: Engine Restart (Brake Release)