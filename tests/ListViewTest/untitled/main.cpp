#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qjsontablemodel.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlContext>

static QJsonTableModel* initModel(QObject* parent = nullptr){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "messages.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    jsonFile.open(QIODevice::ReadOnly);


    QByteArray saveData = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);

    QJsonTableModel::Header header;

    header.push_back( QJsonTableModel::Heading( { {"title","FirstField"},    {"index","FirstField"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","Ref"},   {"index","Ref"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","SecondField"},  {"index","SecondField"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","_id Date"}, {"index","_id"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","date"},   {"index","date"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","message"},  {"index","message"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","token"}, {"index","token"} }) );

    QJsonTableModel * msgModel = new QJsonTableModel( header, parent );
    msgModel->setJson(jsonDocument);

    return msgModel;

}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    qmlRegisterType<QJsonTableModel>("QJsonTableModel", 1, 0, "QJsonTableModel");


    QJsonTableModel* messagesModel = initModel();
    messagesModel->setUserUuid("40655f80-6870-4780-98b3-70d0f1d054dd");

    qDebug() << messagesModel->rowCount();

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    engine.rootContext()->setContextProperty("msgModel", messagesModel);

    //QObject * sendBtn = engine.rootContext()->findChild<QObject*>("SendButton");
    QObject* sendBtn = engine.rootObjects()[0]->findChild<QObject*>("SendButton");
    if(sendBtn){
        QObject::connect(sendBtn, SIGNAL(newMessage(QString)), messagesModel, SLOT(onNewMessage(QString)));
    }
//    example model
//    void TVTime::on_episodesTableView_doubleClicked(const QModelIndex &index)
//    {
//        QJsonObject object = episodes->getJsonObject( index );
//        qDebug() << object["title"];
//    }

    return app.exec();
}
