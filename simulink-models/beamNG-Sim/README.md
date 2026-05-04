# Simulink Integration with BeamNG.drive

This project demonstrates a real-time integration between Simulink and BeamNG.drive for development and validation of an automotive Start-Stop system.

The simulator exposes vehicle dynamics and driver inputs through UDP communication, allowing Simulink models and embedded C code to execute Software-in-the-Loop (SIL) validation in a realistic driving environment.

## Features

The integration extracts and processes the following signals from the simulation:

* Brake pedal signal
* Accelerator pedal signal
* Engine RPM
* Vehicle speed
* Road inclination
* Fuel consumption
* Instantaneous CO₂ emissions
* Gear position
* Door status
* Seatbelt status

## System Architecture

The workflow is composed of:

1. MATLAB Simulink model running the Start-Stop logic
2. Embedded C code generated from Simulink
3. UDP communication interface with BeamNG.drive
4. Real-time SIL validation environment

The integration allows the vehicle simulator to behave as a virtual ECU test bench for Start-Stop feature validation.

---

# Demonstration Videos

## Data sourcing from simulink

[https://github.com/user-attachments/assets/8f917b70-9e52-4892-8204-16a846267783](https://github.com/user-attachments/assets/8f917b70-9e52-4892-8204-16a846267783)

## Detailed Demo Video

[![Demo Video](https://github.com/user-attachments/assets/f11926ed-dddd-4341-832c-372390cefaf5)](https://drive.google.com/file/d/1GjPoC64JINfgNunAyI_hqf7nIkj5mdin/view)

## SIL Integration with BeamNG.drive

[![SIL Integration](https://github.com/user-attachments/assets/815ac0e5-e1ee-41d8-a525-569fe251d49e)](https://drive.google.com/file/d/1gk0PNEK8OuH7OBEmS789syrs1wmj1T1r/view)

---

# Validated Start-Stop Functionalities

## Main Start-Stop Logic

### Requirements Validated

* [x] Engine shutdown is allowed only if previous vehicle speed exceeded 5 km/h
* [x] Engine shutdown is inhibited if previous vehicle speed did not exceed 5 km/h

### Demonstration

[https://github.com/user-attachments/assets/70090b90-c6dd-4aca-9c30-df79d85d41da](https://github.com/user-attachments/assets/70090b90-c6dd-4aca-9c30-df79d85d41da)

---

## Start-Stop Enable/Disable Button

### Requirements Validated

* [x] User can enable the Start-Stop system
* [x] User can disable the Start-Stop system

### Demonstration

[https://github.com/user-attachments/assets/a915d871-aa8c-4f79-9879-461d12660f55](https://github.com/user-attachments/assets/a915d871-aa8c-4f79-9879-461d12660f55)

---

## Inclination Safety Logic

### Requirements Validated

* [x] Engine shutdown is inhibited when inclination exceeds 8.5° (15%)
* [x] Engine shutdown is allowed when inclination is below 8.5° (15%)

### Demonstration

[https://github.com/user-attachments/assets/e39a88c5-2a21-45e1-9f48-5fa24e5c3510](https://github.com/user-attachments/assets/e39a88c5-2a21-45e1-9f48-5fa24e5c3510)

---

## Gear Validation Logic

### Requirements Validated

* [x] Engine shutdown is allowed when gear is in Drive (D)
* [x] Engine shutdown is inhibited for all gears other than Drive (D)

### Demonstration

[https://github.com/user-attachments/assets/f62c2974-419f-48f9-bfd8-73baf80f2620](https://github.com/user-attachments/assets/f62c2974-419f-48f9-bfd8-73baf80f2620)

---

## Door Safety Logic

### Requirements Validated

* [x] Engine shutdown is allowed when all doors are closed
* [x] Engine shutdown is inhibited when a door is open

### Demonstration

[https://github.com/user-attachments/assets/5e9b6165-c962-4bfc-b669-325a4661e35c](https://github.com/user-attachments/assets/5e9b6165-c962-4bfc-b669-325a4661e35c)

---

## Seatbelt Safety Logic

### Requirements Validated

* [x] Engine shutdown is allowed when the seatbelt is fastened
* [x] Engine shutdown is inhibited when the seatbelt is unfastened

### Demonstration

[https://github.com/user-attachments/assets/40370084-debe-4d20-9904-dfb6f8b367e6](https://github.com/user-attachments/assets/40370084-debe-4d20-9904-dfb6f8b367e6)

---

## Fuel saving and CO2 emission

[![Demo Video](https://github.com/user-attachments/assets/67296988-8928-48a4-91af-6d6e5221ffbb)](https://drive.google.com/file/d/1V-ZF-h4WPSD-D2Ert_Hl5NgWsZE216Sd/view?usp=sharing)

10 minutes ride resulting in 1314.9g avoided CO2 and 26ml gasoline saved

# Technologies Used

* MATLAB Simulink
* Embedded C
* UDP Communication
* SIL (Software-in-the-Loop)
* BeamNG.drive

---
