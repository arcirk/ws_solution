#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "messagelistmodel.h"
#include "selectedusersmodel.h"
#include <QDir>
#include <QJsonObject>
#include <QJsonArray>
#include <QQmlContext>


static MessageListModel* initModel(int itemIndex){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "messages.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    jsonFile.open(QIODevice::ReadOnly);


    QByteArray saveData = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument(QJsonDocument::fromJson(saveData));

    MessageListModel::Header header;

    header.push_back( MessageListModel::Heading( { {"title","FirstField"},    {"index","FirstField"} }) );
    header.push_back( MessageListModel::Heading( { {"title","Ref"},   {"index","Ref"} }) );
    header.push_back( MessageListModel::Heading( { {"title","SecondField"},  {"index","SecondField"} }) );
    header.push_back( MessageListModel::Heading( { {"title","_id Date"}, {"index","_id"} }) );
    header.push_back( MessageListModel::Heading( { {"title","date"},   {"index","date"} }) );
    header.push_back( MessageListModel::Heading( { {"title","message"},  {"index","message"} }) );
    header.push_back( MessageListModel::Heading( { {"title","token"}, {"index","token"} }) );
    header.push_back( MessageListModel::Heading( { {"title","contentType"}, {"index","contentType"} }) );

    MessageListModel * msgModel = new MessageListModel( header, nullptr );
    msgModel->addDocument(jsonDocument, itemIndex);
    msgModel->setDocument(itemIndex);

    return msgModel;

}

static void setEmptyModel(MessageListModel* msgModel, int itemIndex){


    QJsonDocument _jsonDocument = QJsonDocument();
    QJsonArray arr = QJsonArray();
    QJsonObject msg = QJsonObject(); //doc.object();
    msg.insert("FirstField", "f3ccb2f2-d431-11e9-ab42-08606e7d17fa");
    msg.insert("Ref", "d3db7a39-c2b1-4ccb-ab67-a4f82f85a25d");
    msg.insert("SecondField", "d81ade18-fb07-11e2-b8bf-08606e7d17fa");
    msg.insert("_id", 0);
    msg.insert("date", 1630075939);
    msg.insert("message", QString::fromStdString(QString("{\"message\":\"test\"}").toUtf8().toBase64().toStdString() ));
    msg.insert("token", "036a82ff52b0e5725e202ff17a5f82b1a1226141");
    msg.insert("contentType", "TEXT");

    arr.push_front(msg);

    _jsonDocument.setArray(arr);
    msgModel->addDocument(_jsonDocument, itemIndex);

}

static SelectedUsersModel* initUsersModel(QObject* parent = nullptr){

    QJsonObject m_currentJsonObject = QJsonObject();

    QString openFileName = "selectedUsers.json";

    QFileInfo fileInfo(openFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(openFileName);
    jsonFile.open(QIODevice::ReadOnly);


    QByteArray saveData = jsonFile.readAll();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(saveData);

    if(jsonDocument.isNull()){
        QJsonArray arr = QJsonArray();
        jsonDocument.setArray(arr);
    }

    SelectedUsersModel::Header header;

    header.push_back( SelectedUsersModel::Heading( { {"title","uuid"},    {"index","uuid"} }) );
    header.push_back( SelectedUsersModel::Heading( { {"title","name"},   {"index","name"} }) );
    header.push_back( SelectedUsersModel::Heading( { {"title","indexDoc"},   {"index","indexDoc"} }) );

    SelectedUsersModel * msgModel = new SelectedUsersModel( header, parent );

    msgModel->setJson(jsonDocument);

    return msgModel;
}

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    qmlRegisterType<MessageListModel>("MessageListModel", 1, 0, "MessageListModel");
    qmlRegisterType<MessageListModel>("SelectedUsersModel", 1, 0, "SelectedUsersModel");

  //Эмуляция загрузки последнего списка диалогов
    SelectedUsersModel* m_usersModel = initUsersModel();
    int index = m_usersModel->addRow("40655f80-6870-4780-98b3-70d0f1d054dd", "user 0");
    MessageListModel* messagesModel = initModel(index);
    index = m_usersModel->addRow("41655f80-6870-4780-98b3-70d0f1d054dd", "empty user");
    setEmptyModel(messagesModel, index);
    index = m_usersModel->addRow("41655f80-6870-4780-98b3-70d0f1d054dd", "empty user long long long long long long long long");
    setEmptyModel(messagesModel, index);
    messagesModel->setUserUuid("40655f80-6870-4780-98b3-70d0f1d054dd");

    //qDebug() << messagesModel->rowCount();
    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    engine.rootContext()->setContextProperty("usersModel", m_usersModel);

    engine.rootContext()->setContextProperty("msgModel", messagesModel);


    QObject* sendBtn = engine.rootObjects()[0]->findChild<QObject*>("msgBox")->findChild<QObject*>("btnSend");
    if(sendBtn){
        QObject::connect(sendBtn, SIGNAL(newMessage(QString)), messagesModel, SLOT(onNewMessage(QString)));
    }

    return app.exec();
}
