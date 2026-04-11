# Start-Stop System: Software-in-the-Loop (SIL) Simulation

![C++](https://img.shields.io/badge/C++-17%2B-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.x-green.svg)
![MATLAB](https://img.shields.io/badge/MATLAB-Simulink-orange.svg)
![Docker](https://img.shields.io/badge/Docker-DevContainer-blue)

## Visão Geral

Este repositório contém a implementação de um sistema Start-Stop veicular utilizando uma abordagem **Software-in-the-Loop (SIL)**. O sistema recria a tecnologia automotiva de redução de consumo de combustível e emissões, que desliga e religa o motor automaticamente com base em condições operacionais predefinidas (ex: paradas em semáforos).

O projeto integra um modelo de dinâmica veicular e controle lógico com uma Interface Homem-Máquina (HMI) executada nativamente, demonstrando a viabilidade de testes integrados e comunicação em rede para sistemas automotivos embarcados.

## Arquitetura do Sistema

A arquitetura é descentralizada e composta por dois nós principais operando em tempo real:

1. **Planta e Lógica de Controle (Simulink):**
   * Responsável por simular a física do veículo (powertrain, freios, aceleração).
   * Implementa a máquina de estados finitos (Stateflow) contendo a lógica de acionamento/corte do motor.
   * Transmite variáveis de telemetria através de pacotes UDP para a interface de visualização.

2. **Cluster de Instrumentos / HMI (C++ e Qt6/QML):**
   * Aplicação gráfica multiplataforma que renderiza os instrumentos do painel (velocímetro, tacômetro, temperatura, hodômetro e indicadores de status).
   * Recebe as mensagens UDP de forma assíncrona, faz o *parsing* dos dados e atualiza a interface via motor de renderização do Qt.

## Estrutura do Repositório

O repositório integrando automação de build e análise estática:

    Start-Stop-System/
    ├── .devcontainer/       # Configuração de infraestrutura como código (Docker)
    ├── .github/workflows/   # Pipelines de CI (Build automatizado e verificação de formato)
    ├── docs/                # Documentação auxiliar e especificações do projeto
    ├── include/             # Arquivos de cabeçalho (.h/.hpp) da aplicação C++
    ├── simulink-models/     # Modelos .slx (Planta e Controlador Start-Stop)
    ├── src/                 # Código-fonte (.cpp) e assets visuais (QML)
    ├── tests/               # Suíte de testes unitários
    ├── .clang-format        # Regras padronizadas de formatação de código
    ├── Makefile             # Automação de compilação local
    └── README.md            # Documentação principal

## Tecnologias e Ferramentas

* **Simulação Matemática:** MATLAB / Simulink (R2024a ou superior recomendado).
* **Desenvolvimento Backend/HMI:** C++17, Qt 6 (QML e Qt Quick).
* **Comunicação:** Protocolo UDP (User Datagram Protocol).
* **Ambiente e DevOps:** Docker, Visual Studio Code (DevContainers), GitHub Actions (CI/CD), CMake/Make, Clang-Format.

## Pré-requisitos

Para executar a simulação completa (SIL), é necessário ter:

* **MATLAB** com as toolboxes *Simulink* e *Stateflow* (Instalado no Windows).
* Para desenvolvimento Linux/C++: **Docker Desktop** e **Visual Studio Code** com a extensão *Dev Containers*.
* Para execução nativa no Windows: **Bibliotecas do Qt6** configuradas ou o pacote de release gerado pelo `windeployqt`.

## Instruções de Compilação e Execução

### Opção A: Executando o Cluster no Windows (Recomendado para Integração SIL)
Como o MATLAB geralmente roda no Windows, executar o HMI nativamente no mesmo sistema operacional facilita a comunicação UDP via `localhost` (127.0.0.1), sem a necessidade de configurar pontes de rede do Docker.

1. Abra o modelo SIL na pasta `Start-Stop-System\simulink-models\StartStopSystem_SIL\SIL.slx`
2. Navegue até o diretório de build do Windows (`src/Cluster_SIL/release_build/`).
3. Dê um duplo clique no executável gerado: `ClusterSIL.exe`.
4. A janela do painel de instrumentos será aberta e ficará aguardando os dados da rede.

### Opção B: Executando/Compilando o Cluster no Linux (DevContainer / Docker)
Ideal para desenvolvimento C++ isolado, validação de portabilidade.

1. Clone o repositório localmente e abra a pasta no VS Code.
2. Quando solicitado, clique em **"Reopen in Container"** para construir o ambiente Docker contendo as bibliotecas de desenvolvimento do Qt6.
3. No terminal do VS Code, compile o código usando o CMake:

       cd src/cluster_SIL/linux_build
       cmake .. -DCMAKE_BUILD_TYPE=Release
       cmake --build . --parallel

4. Execute o binário gerado (requer WSLg ou servidor X11 configurado para renderizar a janela gráfica do container para o host):

       ./ClusterSIL

### Executando a Simulação (Simulink)
Após o HMI (Windows ou Linux) estar em execução:

1. Inicie o MATLAB no seu sistema host.
2. Navegue até o diretório `simulink-models/` e abra o modelo correspondente à simulação SIL.
3. *Nota de Rede:* Se estiver usando o HMI no Windows, o bloco UDP do Simulink deve enviar dados para `127.0.0.1`. Se o HMI estiver no Docker, altere o IP destino no bloco UDP para o IP exposto pelo container (ex: IP do WSL).
4. Inicie a simulação. A telemetria será refletida em tempo real no Cluster HMI.

## Colaboradores

* Andrei Rocha
* Bruno Almeida
* Gabriel Xavier
* Luana Menezes
* Mateus Lourenço
