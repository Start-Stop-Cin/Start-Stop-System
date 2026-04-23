import QtQuick
import QtQuick.Controls
import QtQuick.Window
import ClusterUI

Window {
    id: rootCluster
    width: 800
    height: 350
    visible: true
    title: "Cluster SIL"
    color: themeDarkBg

    // =========================================================================
    // TEMA GLOBAL E VARIÁVEIS COMPARTILHADAS
    // =========================================================================
    property color themeDarkBg: "#151516"
    property color themeArchBorder: "#566587"
    property color themeTrackBorder: "white"
    property color themeNeedle: "#ff8c00"
    property color themeTextPrimary: "white"
    property color themeTextSecondary: "#c19768"
    property color themeCenterDark: "#050508"
    property color themeCenterBorder: "#b3393838"

    property int sharedDialContainerSize: 300
    property int sharedBlueArchSize: 274
    property int sharedBlueArchWidth: 40
    property int sharedOuterTrackSize: 296
    property int sharedInnerTrackSize: 276
    property int sharedTrackBorderWidth: 3
    property real sharedTrackOpacity: 0.8

    property int sharedNeedleWidth: 5
    property int sharedNeedleHeight: 145
    property real sharedNeedleOpacity: 0.8
    property int sharedCenterCircleSize: 140

    // =========================================================================
    // SINAIS / VARIÁVEIS DO VEÍCULO (CONECTADOS AO BACKEND C++)
    // =========================================================================
    property real currentSpeed: Backend.currentSpeed
    property int gearInput: Backend.gearInput
    property real currentRpm: Backend.currentRpm
    property double ss_Enable: Backend.ss_Enable
    property double autostopActive: Backend.autostopActive
    property double autostopAllowed: Backend.autostopAllowed

    // --- NOVO: Conexão da variável do LED ---
    property double hmi_led: Backend.hmi_led

    property string clock: "12:36"
    property int hodometer: 153879
    property int externalTemp: 35

    // LUZES ESPIA E ALERTAS
    property bool doorOpen: Backend.doorOpen
    property bool seatbeltUnfastened: Backend.seatbeltUnfastened
    property int ssStatusInput: Backend.ssStatusInput
    property bool safeStopAlertActive: Backend.safeStopAlertActive

    // VARIÁVEIS DO SIMULINK (CO2 e Combustível)
    property bool show_CO2: Backend.show_CO2
    property real co2_Saved: Backend.co2_Saved
    property bool show_Fuel: Backend.show_Fuel
    property real fuel_Saved: Backend.fuel_Saved

    // LÓGICA DO TEMPORIZADOR SS_ENABLE
    property string ssPopupSource: ""
    Timer {
        id: ssPopupTimer
        interval: 3000
        onTriggered: ssPopupSource = ""
    }

    Connections {
        target: Backend
        function onSs_EnableChanged() {
            ssPopupSource = (Backend.ss_Enable === 1) ? "images/state4.jpeg" : "images/state5.jpeg"
            ssPopupTimer.restart()
        }
    }

    // =========================================================================
    // BARRA SUPERIOR: INDICADOR DE MARCHA
    // =========================================================================
    Item {
        id: gearPositionIndicator
        width: 180
        height: 40
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.horizontalCenter: parent.horizontalCenter

        Rectangle {
            id: outerBorder
            anchors.fill: parent
            color: "#000000"
            radius: 5
            border.color: "#2aaa9898"
            border.width: 2
        }

        Row {
            id: gearRow
            anchors.centerIn: parent
            spacing: 20

            Text {
                text: "P"
                font.pixelSize: 20
                font.bold: rootCluster.gearInput === 0
                color: rootCluster.gearInput === 0 ? rootCluster.themeArchBorder : rootCluster.themeTextPrimary
            }
            Text {
                text: "R"
                font.pixelSize: 20
                font.bold: rootCluster.gearInput === 1
                color: rootCluster.gearInput === 1 ? rootCluster.themeArchBorder : rootCluster.themeTextPrimary
            }
            Text {
                text: "N"
                font.pixelSize: 20
                font.bold: rootCluster.gearInput === 2
                color: rootCluster.gearInput === 2 ? rootCluster.themeArchBorder : rootCluster.themeTextPrimary
            }
            Text {
                text: "D"
                font.pixelSize: 20
                font.bold: rootCluster.gearInput === 3
                color: rootCluster.gearInput === 3 ? rootCluster.themeArchBorder : rootCluster.themeTextPrimary
            }
        }
    }

    // =========================================================================
    // CONJUNTO ESQUERDO: CONTA-GIROS (RPM) E RELÓGIO
    // =========================================================================
    Item {
        id: leftDialContainer
        width: rootCluster.sharedDialContainerSize
        height: rootCluster.sharedDialContainerSize
        anchors.left: parent.left
        anchors.leftMargin: 26
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: leftBlueArch
            width: rootCluster.sharedBlueArchSize
            height: rootCluster.sharedBlueArchSize
            anchors.centerIn: parent
            radius: width / 2
            border.color: rootCluster.themeArchBorder
            color: "transparent"
            border.width: rootCluster.sharedBlueArchWidth
        }

        Rectangle {
            id: leftOuterTrack
            width: rootCluster.sharedOuterTrackSize
            height: rootCluster.sharedOuterTrackSize
            opacity: rootCluster.sharedTrackOpacity
            anchors.centerIn: parent
            radius: width / 2
            color: "transparent"
            border.color: rootCluster.themeTrackBorder
            border.width: rootCluster.sharedTrackBorderWidth
        }

        Rectangle {
            id: leftInnerTrack
            width: rootCluster.sharedInnerTrackSize
            height: rootCluster.sharedInnerTrackSize
            opacity: rootCluster.sharedTrackOpacity
            anchors.centerIn: parent
            radius: width / 2
            color: "transparent"
            border.color: rootCluster.themeTrackBorder
            border.width: rootCluster.sharedTrackBorderWidth
        }

        Repeater {
            id: leftTicksRepeater
            model: 15
            delegate: Item {
                width: rootCluster.sharedOuterTrackSize
                height: rootCluster.sharedOuterTrackSize
                anchors.centerIn: parent
                rotation: -130 + (index * (260 / 14))

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 0
                    width: index % 2 === 0 ? 3 : 1.5
                    height: index % 2 === 0 ? (rootCluster.sharedOuterTrackSize - rootCluster.sharedInnerTrackSize) / 2 : 5
                    color: rootCluster.themeTrackBorder
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 12
                    text: index % 2 === 0 ? (index / 2).toString() : ""
                    color: rootCluster.themeTextPrimary
                    font.pixelSize: 22
                    rotation: -parent.rotation
                }
            }
        }

        Rectangle {
            id: leftRpmNeedle
            width: rootCluster.sharedNeedleWidth
            height: rootCluster.sharedNeedleHeight
            opacity: rootCluster.sharedNeedleOpacity
            color: rootCluster.themeNeedle
            x: parent.width / 2 - width / 2
            y: parent.height / 2 - height
            transformOrigin: Item.Bottom
            rotation: -130 + (rootCluster.currentRpm / 7000) * 260
            Behavior on rotation { SpringAnimation { spring: 1; damping: 0.3 } }
        }

        Rectangle {
            width: rootCluster.sharedCenterCircleSize
            height: rootCluster.sharedCenterCircleSize
            anchors.centerIn: parent
            color: rootCluster.themeCenterDark
            radius: width / 2
            border.color: rootCluster.themeCenterBorder
            border.width: 2
        }

        Text {
            anchors.centerIn: parent
            text: rootCluster.clock
            color: rootCluster.themeTextPrimary
            font.pixelSize: 20
            anchors.verticalCenterOffset: -32
            font.bold: true
        }

        Text {
            anchors.top: parent.verticalCenter
            anchors.topMargin: 25
            anchors.horizontalCenter: parent.horizontalCenter
            text: "RPM\nx1000"
            color: rootCluster.themeTextSecondary
            font.pixelSize: 10
            horizontalAlignment: Text.AlignHCenter
        }
    }

    // =========================================================================
    // ÁREA CENTRAL: VELOCÍMETRO DIGITAL, HODÔMETRO, CO2 E COMBUSTÍVEL
    // =========================================================================
    Item {
        id: centerInfoContainer
        width: 162
        height: 278
        anchors.centerIn: parent

        Text {
            id: centerDigitalSpeed
            anchors.centerIn: parent
            color: rootCluster.themeTextPrimary
            text: rootCluster.currentSpeed.toFixed(0)
            font.pixelSize: 85
            anchors.verticalCenterOffset: -15
        }

        Text {
            anchors.top: centerDigitalSpeed.bottom
            anchors.topMargin: -20
            anchors.horizontalCenter: parent.horizontalCenter
            text: "km/h"
            color: rootCluster.themeTextSecondary
            font.pixelSize: 14
        }

        Text {
            id: fuelMessage
            anchors.bottom: co2Message.top
            anchors.bottomMargin: 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Fuel Saved: " + rootCluster.fuel_Saved.toFixed(3) + " l"
            color: "#2196F3"
            font.pixelSize: 10
            font.bold: true
            opacity: rootCluster.show_Fuel ? 1.0 : 0.0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: 400 } }
        }

        Text {
            id: co2Message
            anchors.bottom: centerOdometer.top
            anchors.bottomMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter
            text: "CO₂ Economy: " + rootCluster.co2_Saved.toFixed(1) + " g"
            color: "#4CAF50"
            font.pixelSize: 10
            font.bold: true
            opacity: rootCluster.show_CO2 ? 1.0 : 0.0
            visible: opacity > 0
            Behavior on opacity { NumberAnimation { duration: 400 } }
        }

        Text {
            id: centerOdometer
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 35
            anchors.horizontalCenter: parent.horizontalCenter
            text: rootCluster.hodometer + " km"
            color: rootCluster.themeTextPrimary
            font.pixelSize: 12
        }

        Image {
            id: safeStop
            anchors.centerIn: parent
            width: 120
            height: 120
            source: "images/SafeStop.jpeg"
            fillMode: Image.PreserveAspectFit
            visible: rootCluster.safeStopAlertActive
        }
    }

    // =========================================================================
    // CONJUNTO DIREITO: VELOCÍMETRO ANALÓGICO E TEMPERATURA
    // =========================================================================
    Item {
        id: rightDialContainer
        width: rootCluster.sharedDialContainerSize
        height: rootCluster.sharedDialContainerSize
        anchors.right: parent.right
        anchors.rightMargin: 26
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: rightBlueArch
            width: rootCluster.sharedBlueArchSize
            height: rootCluster.sharedBlueArchSize
            anchors.centerIn: parent
            radius: width / 2
            color: "transparent"
            border.color: rootCluster.themeArchBorder
            border.width: rootCluster.sharedBlueArchWidth
        }

        Rectangle {
            id: rightOuterTrack
            width: rootCluster.sharedOuterTrackSize
            height: rootCluster.sharedOuterTrackSize
            anchors.centerIn: parent
            radius: width / 2
            color: "transparent"
            border.color: rootCluster.themeTrackBorder
            border.width: rootCluster.sharedTrackBorderWidth
            opacity: rootCluster.sharedTrackOpacity
        }

        Rectangle {
            id: rightInnerTrack
            width: rootCluster.sharedInnerTrackSize
            height: rootCluster.sharedInnerTrackSize
            anchors.centerIn: parent
            radius: width / 2
            color: "transparent"
            border.color: rootCluster.themeTrackBorder
            border.width: rootCluster.sharedTrackBorderWidth
            opacity: rootCluster.sharedTrackOpacity
        }

        Repeater {
            model: 49
            delegate: Item {
                width: rootCluster.sharedOuterTrackSize
                height: rootCluster.sharedOuterTrackSize
                anchors.centerIn: parent
                rotation: -130 + (index * (260 / 48))

                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 0
                    width: index % 2 === 0 ? 3 : 1.5
                    height: index % 2 === 0 ? (rootCluster.sharedOuterTrackSize - rootCluster.sharedInnerTrackSize) / 2 : 5
                    color: rootCluster.themeTrackBorder
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 20
                    text: index % 4 === 0 ? (index / 2 * 10).toString() : ""
                    color: rootCluster.themeTextPrimary
                    font.pixelSize: 16
                    rotation: -parent.rotation
                }
            }
        }

        Rectangle {
            id: rightSpeedNeedle
            width: rootCluster.sharedNeedleWidth
            height: rootCluster.sharedNeedleHeight
            opacity: rootCluster.sharedNeedleOpacity
            color: rootCluster.themeNeedle
            x: parent.width / 2 - width / 2
            y: parent.height / 2 - height
            transformOrigin: Item.Bottom
            rotation: -130 + (rootCluster.currentSpeed / 240) * 260
            Behavior on rotation { SpringAnimation { spring: 1; damping: 0.3 } }
        }

        Rectangle {
            width: rootCluster.sharedCenterCircleSize
            height: rootCluster.sharedCenterCircleSize
            anchors.centerIn: parent
            color: rootCluster.themeCenterDark
            radius: width / 2
            border.color: rootCluster.themeCenterBorder
            border.width: 2
        }

        Text {
            anchors.centerIn: parent
            text: rootCluster.externalTemp + "°C"
            color: rootCluster.themeTextPrimary
            font.pixelSize: 20
            anchors.verticalCenterOffset: -38
            font.bold: true
        }

        Text {
            color: rootCluster.themeTextSecondary
            text: "km/h"
            anchors.top: parent.verticalCenter
            anchors.topMargin: 25
            font.pixelSize: 10
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    // =========================================================================
    // BARRA INFERIOR: LUZES DE AVISO (LÓGICA PRIORITÁRIA INTEGRADA)
    // =========================================================================
    Row {
        id: bottomIndicatorRow
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 25

        Image {
            width: 50; height: 50
            source: "images/state1.jpeg" // Porta
            fillMode: Image.PreserveAspectFit
            visible: !rootCluster.doorOpen
        }

        Image {
            width: 50; height: 50
            source: "images/state2.jpeg" // Cinto
            fillMode: Image.PreserveAspectFit
            visible: !rootCluster.seatbeltUnfastened
        }

        Item {
            width: 50; height: 50

            // Prioridade: Popup de Enable/Disable
            Image {
                anchors.fill: parent; source: rootCluster.ssPopupSource; visible: rootCluster.ssPopupSource !== ""; fillMode: Image.PreserveAspectFit
            }

            // Estados do Sistema
            Image {
                source: "images/state6.jpeg";
                anchors.fill: parent;
                visible: rootCluster.ssPopupSource === "" && rootCluster.autostopActive === 1;
                fillMode: Image.PreserveAspectFit
            }

            Image {
                source: "images/state3.jpeg"
                anchors.fill: parent
                visible: rootCluster.ssPopupSource === "" && rootCluster.autostopAllowed === 0 && rootCluster.autostopActive === 0 && rootCluster.ss_Enable === 1
                fillMode: Image.PreserveAspectFit
            }
        }
    }

    // =========================================================================
    // INDICADOR HMI LED (Start Stop Disabled)
    // =========================================================================
    Row {
        id: hmiLedIndicator
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 15
        anchors.rightMargin: 15
        spacing: 8
        opacity: 0.6
        visible: rootCluster.hmi_led === 1

        Rectangle {
            width: 10
            height: 10
            radius: 5
            color: "#ff3333"
            anchors.verticalCenter: parent.verticalCenter
            border.color: "#8b0000"
            border.width: 1
        }

        Text {
            text: "Start Stop Disabled"
            color: rootCluster.themeTextSecondary
            font.pixelSize: 11
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}