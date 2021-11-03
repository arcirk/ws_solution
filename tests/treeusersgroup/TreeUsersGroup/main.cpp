#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>

QStandardItemModel* getUsersGroupModel(){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "messages.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    jsonFile.open(QIODevice::ReadOnly);


    QByteArray saveData = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument(QJsonDocument::fromJson(saveData));



}


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
