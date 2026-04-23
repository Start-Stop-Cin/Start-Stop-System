#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "ClusterBackend.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	// Instancia backend de comunicação
	ClusterBackend backend;

	QQmlApplicationEngine engine;

	// Expõe classe C++ para o QML com o nome "Backend"
	engine.rootContext()->setContextProperty("Backend", &backend);

	// Caminho da interface simplificado
	const QUrl url("qrc:/ClusterUI/ui/main.qml");
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
						 if (!obj && url == objUrl)
							 QCoreApplication::exit(-1);
					 }, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}