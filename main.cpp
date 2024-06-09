#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QtCore>
#include "initvaluesmodel.h"
#include "stationlistmodel.h"
#include "filelistmodel.h"

QMediaPlayer* player = nullptr;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qputenv("ANDROID_OPENSSL_SUFFIX", "_3");
    qputenv("QML_COMPAT_RESOLVE_URLS_ON_ASSIGNMENT", "1");
    qputenv("QT_QUICK_CONTROLS_CONF", ":/qtquickcontrols2.conf");
    QSettings settings;
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") &&
        settings.value("style").toString().isEmpty()) {
    #if defined(Q_OS_MACOS)
        QQuickStyle::setStyle(QString("iOS"));
    #elif defined(Q_OS_IOS)
        QQuickStyle::setStyle(QString("iOS"));
    #elif defined(Q_OS_WINDOWS)
        QQuickStyle::setStyle(QString("Windows"));
   #elif defined(Q_OS_LINUX)
        QQuickStyle::setStyle(QString("Material"));
    #elif defined(Q_OS_ANDROID)
        QQuickStyle::setStyle(QString("Material"));
#endif
    } else {
        QQuickStyle::setStyle(settings.value("style").toString());
    }
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

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("audioplayer", "Main");

    return app.exec();
}
