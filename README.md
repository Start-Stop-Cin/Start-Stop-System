<<<<<<< Updated upstream
# Projeto Start-Stop Veicular

## Contexto

O sistema Start-Stop veicular é uma tecnologia desenvolvida para reduzir o consumo de combustível e as emissões de poluentes em situações de parada temporária, como semáforos ou congestionamentos. O princípio de funcionamento consiste em desligar automaticamente o motor quando o veículo está parado e religá-lo quando o condutor aciona a embreagem (veículos manuais) ou libera o freio (veículos automáticos).

Essa tecnologia é amplamente utilizada na indústria automotiva, estando presente em modelos de fabricantes como Fiat, Volkswagen e Ford, contribuindo para maior eficiência energética e atendimento a normas ambientais.

## Objetivo do Projeto

Este projeto tem como objetivo desenvolver um modelo funcional de um sistema Start-Stop veicular com Simulink que:

- Monitore variáveis como velocidade, estado do pedal de freio/embreagem e nível de bateria;
- Determine as condições ideais para desligamento do motor;
- Realize o religamento seguro e eficiente do motor;
- Garanta conforto ao usuário e segurança operacional.

## Estrutura do Projeto

```
Start-Stop-System/
├── README.md              # Documentação principal
├── simulink-models/       # Modelos Simulink

```

## Pasta simulink-models

Esta pasta concentra os arquivos de modelagem e simulação do sistema Start-Stop no Simulink.

Nas próximas entregas, a estrutura será expandida conforme as novas etapas forem sendo concluídas.

## Como Usar (Etapa Atual)

### Requisitos
- MATLAB/Simulink (versão recomendada: R2024a ou posterior)

## Co-autores

- Andrei Rocha
- Bruno Almeida
- Gabriel Xavier
- Luana Menezes
- Mateus Lourenço

=======
# Start-Stop System: Embedded Software + SIL Integration

![C](https://img.shields.io/badge/C-Embedded-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.x-green.svg)
![MATLAB](https://img.shields.io/badge/MATLAB-Simulink-orange.svg)
![Docker](https://img.shields.io/badge/Docker-DevContainer-blue)

---

## Overview

This repository contains the implementation of a **Start-Stop automotive system** using a modular embedded software architecture in C, validated through a **Software-in-the-Loop (SIL)** environment.

The system simulates real automotive functionality responsible for reducing fuel consumption and emissions by automatically stopping and restarting the engine based on vehicle conditions.

The project integrates:

* Embedded software modules written in **C**
* A **Simulink SIL model** for validation and testing
* A **Cluster HMI (Qt/QML)** for visualization
* Optional **vehicle simulation and UDP communication**

---

## System Architecture

The system follows a modular architecture divided into three main layers:

### 1. Embedded Software (C – `src/`)

Core logic of the Start-Stop system:

* State machine (operation logic)
* Safety and inhibit conditions
* Input processing
* Timing management
* CO₂ avoided estimation
* Fuel saving estimation
* Application integration layer

Main execution entry point:

```c
SS_App_Run10ms_If(...)
```

Executed every **10 ms**, simulating an ECU task.

---

### 2. SIL Environment (Simulink – `simulink-models/`)

Responsible for:

* Providing interactive inputs (dashboard)
* Executing the embedded software
* Validating system behavior
* Visualizing outputs

Features validated:

* AutoStop / AutoStart logic
* SafeStop behavior
* Drive cycle memory
* Timer-based restart
* CO₂ avoided
* Fuel saved

---

### 3. Cluster HMI (Qt/QML – `src/`)

Graphical interface simulating a vehicle instrument cluster:

* Displays vehicle states and signals
* Receives data via UDP
* Visualizes:

  * Speed, gear, ignition
  * Start-Stop status
  * CO₂ avoided
  * Fuel saved

---

## Repository Structure

```text
Start-Stop-System/
├── .devcontainer/         # Docker-based development environment
├── .github/workflows/     # CI pipelines (build, format, checks)
├── .vscode/               # VS Code configuration
├── docs/                  # Documentation and reports
├── include/               # Header files (.h)
├── simulink-models/       # SIL and model-based simulations (.slx)
├── src/                   # C source code + Cluster (Qt/QML)
│   ├── ss_operation.c
│   ├── ss_timer.c
│   ├── ss_co2.c
│   ├── ss_fuel.c
│   ├── ss_input_proc.c
│   ├── ss_control.c
│   ├── ss_app.c
│   └── cluster (Qt application)
├── tests/                 # Unit tests
├── Makefile               # Build automation
├── .clang-format          # Code formatting rules
└── README.md              # Project documentation
```

---

## Embedded Software Modules

| Module          | Description                                     |
| --------------- | ----------------------------------------------- |
| `ss_operation`  | State machine (VehicleOff / Running / AutoStop) |
| `ss_inhibit`    | Safety and inhibit conditions                   |
| `ss_input_proc` | Signal processing and conditioning              |
| `ss_timer`      | AutoStop timing and timeout logic               |
| `ss_co2`        | CO₂ avoided estimation                          |
| `ss_fuel`       | Fuel saving estimation                          |
| `ss_control`    | Start-Stop button and enable logic              |
| `ss_app`        | Integration layer (main execution)              |

---

## Key Features

* Start-Stop state machine
* Safety and inhibit logic
* SafeStop behavior
* Drive cycle memory
* Timer-based restart
* CO₂ avoided estimation
* Fuel saving estimation
* HMI integration
* SIL validation environment

---

## Technologies

* **Embedded Software:** C
* **Simulation:** MATLAB / Simulink
* **HMI:** Qt 6 (QML)
* **Communication:** UDP
* **DevOps:** Docker, GitHub Actions, Makefile

---

## How to Run

### 1. Run SIL Simulation

1. Open MATLAB
2. Add project to path:

   ```matlab
   addpath(genpath('path_to_project'))
   ```

3. Open:

   ```
   simulink-models/StartStopSystem_SIL/sil.slx
   ```

4. Start simulation
5. Use dashboard controls to interact

---

### 2. Run Cluster (Windows – Recommended)

1. Navigate to:

   ```
   src/Cluster_SIL/release_build/
   ```

2. Run:

   ```
   ClusterSIL.exe
   ```

3. Start Simulink simulation

---

### 3. Run Cluster (Linux / Docker)

```bash
cd src/cluster_SIL/linux_build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
./ClusterSIL
```

---

## SIL Integration

The SIL model integrates the embedded C software via:

```c
SS_App_Run10ms_If(...)
```

Execution flow:

```text
Dashboard Inputs
        ↓
Simulink
        ↓
C Interface (App Layer)
        ↓
System Modules
        ↓
Outputs → Dashboard / Cluster
```

---

## Validation

The system is validated using SIL through:

* Interactive dashboard scenarios
* Real-time signal observation
* Functional verification of:

  * State transitions
  * Safety conditions
  * Timing logic
  * Estimation features

---

## Purpose

This project demonstrates:

* Embedded software design for automotive systems
* Integration between C code and model-based environments
* SIL validation methodology
* HMI integration and visualization

---

## Limitations

* Simplified vehicle dynamics
* Estimations based on calibrated assumptions
* Not intended for production deployment

---

## Contributors

* Andrei Rocha
* Bruno Almeida
* Gabriel Xavier
* Luana Menezes
* Mateus Lourenço
>>>>>>> Stashed changes
