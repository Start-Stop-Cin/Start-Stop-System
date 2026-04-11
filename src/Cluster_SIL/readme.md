# Digital Cluster HMI - SIL Simulation

This repository contains a **Digital Instrument Cluster (HMI)** developed in **Qt 6 (C++/QML)**. This project was designed to act as a real-time visualization interface for a **Software-in-the-Loop (SIL)** simulation executed in **Simulink/MATLAB**.

The integration between the vehicle's dynamic model and the graphical interface is performed via the **UDP protocol**, ensuring low latency and precise updates for the instruments (speedometer, tachometer, and indicators).

---

## How to Run the Project

Link to demo video: <https://drive.google.com/file/d/1GR1ehBuGSHFUp2576c1T3QHjzxmZbEwR/view?usp=sharing>

You do not need to have Qt installed to run this system, as all necessary dependencies are included in the distribution folder.

### 1. Launch the Interface (HMI)

The pre-compiled executable is ready for use in a Windows environment.

1. Navigate to the folder:  
   `src/Cluster_SIL/release_build/`
2. Run the file:  
   **`ClusterSIL.exe`**
   * *Note: If the Windows Firewall requests permission, authorize access so the program can receive data coming from Simulink.*

### 2. Start the Simulation in Simulink

The dashboard will start responding as soon as it receives data packets.

1. Open **MATLAB** and load your **Simulink** model (located in the project's simulation folder).
2. Verify that the **UDP Send** block in the model is configured to:
   * **IP:** `127.0.0.1` (localhost)
   * **Port:** `55000`
3. Click the **Run** button inside the SIL model in Simulink.
4. As soon as the simulation starts, data will be sent to `ClusterSIL.exe`, and you will see the interface updating in real time.

---

## Data Structure (UDP)

The HMI expects to receive a **120-byte packet** composed of **15 values of type `double`** (8 bytes each), in the following order:

| Index | Signal | Description |
|:---:|:---|:---|
| 0 | Speed | Current vehicle speed (km/h) |
| 1 | Gear Input | Selected gear (P, R, N, D) |
| 2 | Door Open | Door status (Boolean) |
| 3 | Seatbelt | Seatbelt unfastened status (Boolean) |
| 4 | SafeStop | SafeStop alert active |
| 5-8 | Eco Data | CO2 and Fuel economy information |
| 9-12| Start-Stop | Start-Stop system status and conditions |
| 13 | RPM | Engine revolutions per minute |
| 14 | HMI LED | Interface feedback LED status |

---

## Technologies Used

* **Qt 6.11.0 (MinGW 64-bit)**: Backend logic and UDP processing.
* **QML / Qt Quick**: High-performance graphical interface and animations.
* **Simulink**: Vehicle dynamic modeling and control logic.
* **Windeployqt**: Windows dependency packaging tool.

---

## Notes

* Ensure the Simulink simulation is running on the same machine as the executable, or adjust the IP in the UDP block if using different computers.
* The executable was optimized in **Release** mode to ensure the interface does not freeze during high-frequency data reception.
