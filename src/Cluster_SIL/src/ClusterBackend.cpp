#include "ClusterBackend.h"
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <QDebug>

ClusterBackend::ClusterBackend(QObject *parent) : QObject(parent) {
	udpSocket = new QUdpSocket(this);
	// Bind na porta 55000 para receber os dados do Simulink
	udpSocket->bind(QHostAddress::AnyIPv4, 55000);
	connect(udpSocket, &QUdpSocket::readyRead, this, &ClusterBackend::readPendingDatagrams);
}

void ClusterBackend::readPendingDatagrams() {
	while (udpSocket->hasPendingDatagrams()) {
		QNetworkDatagram datagram = udpSocket->receiveDatagram();
		QByteArray data = datagram.data();

		// Verifica se o pacote tem o tamanho esperado (15 doubles = 120 bytes)
		if (data.size() >= 15 * sizeof(double)) {
			double *values = reinterpret_cast<double*>(data.data());

			/* COMENTADO PARA EVITAR TRAVAMENTO DA GUI (Não Respondendo)
			   Imprimir muitas mensagens no console trava a interface no Windows.
			*/

			// Atualização dos sinais para a interface QML
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
			updateValue(m_autostopAllowed, values[12], &ClusterBackend::autostopAllowedChanged);
			updateValue(m_currentRpm, values[13], &ClusterBackend::currentRpmChanged);
			updateValue(m_hmi_led, values[14], &ClusterBackend::hmi_ledChanged);

		} else {
			qDebug() << "FALHA: Pacote com tamanho incorreto:" << data.size();
		}
	}
}