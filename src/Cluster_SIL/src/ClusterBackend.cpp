#include "ClusterBackend.h"
#include <QNetworkDatagram>
#include <QDebug>

ClusterBackend::ClusterBackend(QObject *parent) : QObject(parent) {
	udpSocket = new QUdpSocket(this);
	udpSocket->bind(QHostAddress::AnyIPv4, 55000);
	connect(udpSocket, &QUdpSocket::readyRead, this, &ClusterBackend::readPendingDatagrams);
}

void ClusterBackend::readPendingDatagrams() {
	while (udpSocket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = udpSocket->receiveDatagram();
		QByteArray data = datagram.data();

		// Ajustado para 12 elementos (0 a 11)
		if (data.size() >= 12 * sizeof(double)) {
			double *values = reinterpret_cast<double*>(data.data());

			qDebug() << "------------------------------------------";
			qDebug() << "PACOTE RECEBIDO (" << data.size() << " bytes):";
			qDebug() << "[0] Velocidade:      " << values[0];
			qDebug() << "[1] Gear Input:      " << values[1];
			qDebug() << "[2] Door Open:       " << values[2];
			qDebug() << "[3] Seatbelt Unf.:   " << values[3];
			qDebug() << "[4] SafeStop Alert:  " << values[4];
			qDebug() << "[5] Show CO2:        " << values[5];
			qDebug() << "[6] CO2 Saved:       " << values[6];
			qDebug() << "[7] Show Fuel:       " << values[7];
			qDebug() << "[8] Fuel Saved:      " << values[8];
			qDebug() << "[9] SS Status Input: " << values[9];
			qDebug() << "[10] SS Enable:      " << values[10];
			qDebug() << "[11] Autostop Active:" << values[11];
			qDebug() << "------------------------------------------";

			updateValue(m_currentSpeed, values[0], &ClusterBackend::currentSpeedChanged);
			updateValue(m_gearInput, values[1], &ClusterBackend::gearInputChanged);
			updateValue(m_doorOpen, values[2], &ClusterBackend::doorOpenChanged);
			updateValue(m_seatbeltUnfastened, values[3], &ClusterBackend::seatbeltUnfastenedChanged);
			updateValue(m_safeStopAlertActive, values[4], &ClusterBackend::safeStopAlertActiveChanged);
			updateValue(m_show_CO2, values[5], &ClusterBackend::show_CO2Changed);
			updateValue(m_co2_Saved, values[6], &ClusterBackend::co2_SavedChanged);
			updateValue(m_show_Fuel, values[7], &ClusterBackend::show_FuelChanged);
			updateValue(m_fuel_Saved, values[8], &ClusterBackend::fuel_SavedChanged);
			updateValue(m_ssStatusInput, values[9], &ClusterBackend::ssStatusInputChanged);
			updateValue(m_ss_Enable, values[10], &ClusterBackend::ss_EnableChanged);
			updateValue(m_autostopActive, values[11], &ClusterBackend::autostopActiveChanged);

		} else {
			qDebug() << "Recebi um pacote, mas o tamanho esta errado. Tamanho:" << data.size();
		}
	}
}