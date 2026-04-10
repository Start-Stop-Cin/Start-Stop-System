Inside `StartStopSystem_Model`:

- `model.slx` is the **top-level integrated model**
- The remaining `.slx` files are the subsystem models used by the integrated model through Model Reference

---

## Overview

The integrated model connects multiple subsystems responsible for:

- Input conditioning (AutoStop / AutoRestart)
- Safety and inhibition logic
- State machine behavior
- HMI / Cluster interface
- CO₂ estimation

The system follows a modular design to improve readability, scalability, and traceability.

---

## Setup

Before running the model, ensure all files are accessible in MATLAB.

### Add folder to MATLAB path

Home → Set Path → Add Folder (including subfolders)## Running the model

- Open the integrated model: 
 ```bash
open('model.slx')
```

- Run the simulation using the Simulink interface or launch the dashboard  StartStopApp

 ##  Model Reference

This project uses Model Reference blocks.

Important:
All .slx files must be available in the MATLAB path.
Otherwise, referenced models will not be loaded correctly.

## Requirements Traceability

Requirements are linked to model elements using Simulink Requirements.

To visualize:

Apps → Requirements Manager

or

Requirements → Show Links

