#ifndef CLUSTERBACKEND_H
#define CLUSTERBACKEND_H

#include <QObject>
#include <QUdpSocket>
#include <QtQml/qqml.h>

class ClusterBackend : public QObject {
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	// Propriedades expostas para o QML
	Q_PROPERTY(double currentSpeed READ currentSpeed NOTIFY currentSpeedChanged)
	Q_PROPERTY(double gearInput READ gearInput NOTIFY gearInputChanged)
	Q_PROPERTY(double doorOpen READ doorOpen NOTIFY doorOpenChanged)
	Q_PROPERTY(double seatbeltUnfastened READ seatbeltUnfastened NOTIFY seatbeltUnfastenedChanged)
	Q_PROPERTY(double safeStopAlertActive READ safeStopAlertActive NOTIFY safeStopAlertActiveChanged)
	Q_PROPERTY(double show_CO2 READ show_CO2 NOTIFY show_CO2Changed)
	Q_PROPERTY(double co2_Saved READ co2_Saved NOTIFY co2_SavedChanged)
	Q_PROPERTY(double show_Fuel READ show_Fuel NOTIFY show_FuelChanged)
	Q_PROPERTY(double fuel_Saved READ fuel_Saved NOTIFY fuel_SavedChanged)
	Q_PROPERTY(double ssStatusInput READ ssStatusInput NOTIFY ssStatusInputChanged)

  public:
	explicit ClusterBackend(QObject *parent = nullptr);

	// Getters
	double currentSpeed() const { return m_currentSpeed; }
	double gearInput() const { return m_gearInput; }
	double doorOpen() const { return m_doorOpen; }
	double seatbeltUnfastened() const { return m_seatbeltUnfastened; }
	double safeStopAlertActive() const { return m_safeStopAlertActive; }
	double show_CO2() const { return m_show_CO2; }
	double co2_Saved() const { return m_co2_Saved; }
	double show_Fuel() const { return m_show_Fuel; }
	double fuel_Saved() const { return m_fuel_Saved; }
	double ssStatusInput() const { return m_ssStatusInput; }

  signals:
	// Sinais emitidos quando o valor muda
	void currentSpeedChanged();
	void gearInputChanged();
	void doorOpenChanged();
	void seatbeltUnfastenedChanged();
	void safeStopAlertActiveChanged();
	void show_CO2Changed();
	void co2_SavedChanged();
	void show_FuelChanged();
	void fuel_SavedChanged();
	void ssStatusInputChanged();

  private slots:
	void readPendingDatagrams();

  private:
	QUdpSocket *udpSocket;

	// Variáveis internas
	double m_currentSpeed = 0;
	double m_gearInput = 0;
	double m_doorOpen = 0;
	double m_seatbeltUnfastened = 0;
	double m_safeStopAlertActive = 0;
	double m_show_CO2 = 0;
	double m_co2_Saved = 0;
	double m_show_Fuel = 0;
	double m_fuel_Saved = 0;
	double m_ssStatusInput = 0;

	// Função auxiliar (agora usando template para respeitar o C++17)
	template <typename Signal>
	void updateValue(double &member, double newValue, Signal signal) {
		if (member != newValue) {
			member = newValue;
			emit (this->*signal)();
		}
	}
};

#endif // CLUSTERBACKEND_H