cmake_minimum_required(VERSION 3.16)
project(ClusterSIL VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(COMMAND qt_policy)
    qt_policy(SET QTP0001 NEW)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick Network)

qt_add_executable(ClusterSIL
    src/main.cpp
)

# AJUDA O QT A ENCONTRAR O ARQUIVO .h DENTRO DA PASTA SRC
target_include_directories(ClusterSIL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

qt_add_qml_module(ClusterSIL
    URI ClusterUI
    VERSION 1.0
    SOURCES
        src/ClusterBackend.cpp
        src/ClusterBackend.h
    QML_FILES 
        ui/main.qml
    RESOURCES
        ui/images/state1.jpeg
        ui/images/state2.jpeg
        ui/images/state3.jpeg
        ui/images/state4.jpeg
        ui/images/state5.jpeg
        ui/images/state6.jpeg
        ui/images/SafeStop.jpeg
)

target_link_libraries(ClusterSIL PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::Network
)