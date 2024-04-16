#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtCore>
#include "initvaluesmodel.h"
#include "stationlistmodel.h"
#include "filelistmodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qRegisterMetaType<data>();
    InitvaluesModel* ivm = InitvaluesModel::instance();
    emit ivm->operate("init.json");        // start thread for reading init.json file

    QTranslator translator;
    if(QLocale::system().name().section(',',0,0).compare("de_DE") == 0) {
        Q_UNUSED(translator.load("i18n_de"));
    } else {
        Q_UNUSED(translator.load("i18n_en"));
    }
    app.installTranslator(&translator);

    engine.rootContext()->setContextProperty("initValuesModel", ivm);
    qmlRegisterType<StationListModel>("org.example", 1, 0, "StationListModel");
    qmlRegisterType<FileListModel>("org.hsoft", 1, 0, "FileListModel");
    const QUrl url(u"qrc:/audioplayer/Main.qml"_qs);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
