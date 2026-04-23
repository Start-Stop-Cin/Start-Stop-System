## Project Organization (Updated)

The project is organized into modular subsystems and integrated models:

* **StartStopSystem_Model/**

  * `model.slx` → Top-level integrated model
  * Subsystems (Model References):

    * `Operation_Module/` → State machine and operation logic
    * `EnableDisable/` → Button handling and SS enable logic
    * `SafetyModule/` → Safety and inhibit conditions
    * `SafeStop/` → Standstill and safe stop logic
    * `CO2/` → CO₂ avoided estimation
    * `ClusterHMI/` → Display and HMI signals

* **StartStopSystem_SIL/**

  * `sil.slx` → Software-in-the-Loop model
  * Integrated with C modules via `SS_App_Run10ms_If`

* **VehicleModel/**

  * Simplified vehicle dynamics for simulation

* **beamNG-Sim/**

  * External simulation integration (optional)

