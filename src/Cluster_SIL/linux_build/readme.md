# Procedimento para construção do executável


# Instala Dependencias

sudo apt update && sudo apt install -y build-essential cmake qt6-base-dev qt6-declarative-dev qt6-tools-dev

ou 

sudo apt update && sudo apt install -y build-essential cmake qt6-base-dev qt6-declarative-dev qt6-tools-dev libxkbcommon-dev qml6-module-qtquick qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-templates qml6-module-qtquick-window qml6-module-qtqml-workerscript

# Limpeza de Cache

rm -rf CMakeCache.txt CMakeFiles/

cmake .. -DCMAKE_BUILD_TYPE=Release

# Dependencias para a camada visual da HMI

sudo apt update && sudo apt install -y libxkbcommon-dev

# Reconfiguração e compilação

## Limpa o cache para garantir que ele detecte a nova biblioteca
rm -rf CMakeCache.txt CMakeFiles/

## Configura novamente
cmake .. -DCMAKE_BUILD_TYPE=Release

## Compila
cmake --build . --parallel

# Qt6 Runtime

## Limpeza e Atualização do servidor de pacotes

sudo rm -rf /var/lib/apt/lists/*
sudo apt update

## Instalação dos pacotes de QML

sudo apt install -y qml6-module-qtquick qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-templates


## Qt Quick Window

sudo apt update && sudo apt install -y qml6-module-qtquick-window qml6-module-qtqml-workerscript

# Executar Aplicação

./ClusterSIL


# Determinar o Endereço de Rede do Container

hostname -I







# App Deployment 

cd /workspaces/Start-Stop-System/src/Cluster_SIL/linux_build

cmake .. -DCMAKE_BUILD_TYPE=Release && make

export QML_SOURCES_PATHS="/workspaces/Start-Stop-System/src/Cluster_SIL/ui"

../linuxdeploy-x86_64.AppImage --appdir AppDir --plugin qt --output appimage

# Executar aplicação
./ClusterSIL-x86_64.AppImage