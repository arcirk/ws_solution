#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "include/messagelistmodel.h"
#include "include/selectedusersmodel.h"
#include <QDir>
#include <QQmlContext>
#include <QQuickStyle>

#include "include/qmlwebsocket.h"
#include <qproxymodel.h>
#include <qjsontablemodel.h>
#include "userinfo.h"

#include "include/clientsettings.h"

#include <QFileInfo>

[[maybe_unused]] void testSaveArgs(QStringList& arg){

    QString saveFileName = "testArgs.txt";
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    // Записываем текущий объект Json в файл
    jsonFile.write(arg.join(" ; ").toUtf8());
    jsonFile.close();   // Закрываем файл
}

bool fileExists(QString path) {
    QFileInfo check_file(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return check_file.exists() && check_file.isFile();
}

void updateParamsFromArgs(QStringList& arg){

    //testSaveArgs(arg);

    if(arg.count() == 5){

        QString pathToClient = arg[0];
        QString usr = arg[1];
        QString hash = arg[2];
        QString uuidAgent = arg[3];
        QString host = arg[4];
        int port = arg[5].toInt();

       auto * sett = new ClientSettings();
       sett->init();
       sett->ServerHost = host;
       sett->ServerPort = port;
       sett->RootUser = usr;
       sett->Hash = hash;
       sett->AutoConnect = true;
       sett->SaveHash = true;

       sett->setSettingsFileName("config_client.json");

       sett->save_settings();

       delete sett;

    }
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<bWebSocket>("arcirk.bWebSocket", 1, 0, "WebSocket");

    QStringList cmdline_args = QCoreApplication::arguments();

    qDebug() << cmdline_args.length();

    updateParamsFromArgs(cmdline_args);

    QQuickStyle::setStyle("Material");

    qmlRegisterType<MessageListModel>("MessageListModel", 1, 0, "MessageListModel");
    qmlRegisterType<SelectedUsersModel>("SelectedUsersModel", 1, 0, "SelectedUsersModel");
    qmlRegisterType<QJsonTableModel>("QJsonTableModel", 1, 0, "QJsonTableModel");
    qmlRegisterType<QProxyModel>("QProxyModel", 1, 0, "QProxyModel");
    qmlRegisterType<UserInfo>("UserInfo", 1, 0, "UserInfo");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    bool agentUsed = cmdline_args.count() > 1;

    engine.rootContext()->setContextProperty("agentUsed", agentUsed);
    QString uuidAgent;
    if (agentUsed)
        uuidAgent = cmdline_args[3];

    engine.rootContext()->setContextProperty("uuidAgent", uuidAgent);

    engine.load(url);

    //testSaveArgs(cmdline_args);

    return app.exec();
}
