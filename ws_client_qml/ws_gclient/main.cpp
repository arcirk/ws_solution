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

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<bWebSocket>("arcirk.bWebSocket", 1, 0, "WebSocket");

//    if (argc == 3){
//        auto const address = argv[1];
//        auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
//        //auto const user = std::max<int>(1, std::atoi(argv[3]));
//        //qDebug() << "address:" << address << "port: " << port;
//        client->get_settings()->ServerHost = QString(address);
//        client->get_settings()->ServerPort = port;
//    }

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
    engine.load(url);

    return app.exec();
}
