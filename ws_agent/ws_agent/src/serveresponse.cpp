#include "../include/serveresponse.h"

#include <iws_client.h>

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>

ServeResponse::ServeResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

QString ServeResponse::base64_decode(const std::string &resp)
{
    std::string json;
    try {
        json = IClient::base64_decode(resp);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    return QString::fromStdString(json);
}

void ServeResponse::parse(const QString& resp){


    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    if(!_doc.isNull())
    {
        if(_doc.isObject())
        {
            QJsonObject obj = _doc.object();
            auto _message = obj.find("message");
            auto _result = obj.find("result");
            auto _command = obj.find("command");
            auto _recipient = obj.find("uuid_channel");
            auto _uuid = obj.find("user_uuid");
            auto _uuid_session= obj.find("uuid");
            auto _contentType = obj.find("contentType");
            auto _recipientName = obj.find("channel_name");
            auto _app_name = obj.find("app_name");

             if(_message->isString()){
                 message = _message.value().toString();
             }
             if(_command->isString()){
                 command = _command.value().toString();
             }
             if(_result->isString()){
                 result = _result.value().toString();
             }
             if(_recipient->isString()){
                 recipient = _recipient.value().toString();
             }
             if(_uuid->isString()){
                 uuid = _uuid.value().toString();
             }
             if(_contentType->isString()){
                 contentType = _contentType.value().toString();
             }
             if(_contentType->isString()){
                 recipientName = _recipientName.value().toString();
             }
             if(_app_name->isString()){
                 app_name = _app_name.value().toString();
             }
             if(_uuid_session->isString()){
                 uuid_session = _uuid_session.value().toString();
             }
            isParse = true;
        }
    }

}

void ServeResponse::debugSaveResponse(const QString &filename, const QString &json)
{
    //ToDo: временная процедура
    QString saveFileName = filename + ".json";
    QFileInfo fileInfo(saveFileName);
    QDir::setCurrent(fileInfo.path());

    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QJsonDocument _doc(QJsonDocument::fromJson(json.toUtf8()));
    jsonFile.write(_doc.toJson(QJsonDocument::Indented));
    jsonFile.close();

}

QString ServeResponse::to_string() const
{
    QJsonObject msg = QJsonObject();
    msg.insert("message", message);
    msg.insert("command", command);
    msg.insert("result", result);
    msg.insert("recipient", recipient);
    msg.insert("token", token);
    msg.insert("uuid", uuid);
    msg.insert("contentType", contentType);
    msg.insert("recipientName", recipientName);
    msg.insert("app_name", app_name);
    msg.insert("uuid_session", uuid_session);
    return QJsonDocument(msg).toJson(QJsonDocument::Indented);
}

QJsonDocument ServeResponse::parseResp(const QString &resp){

    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    return _doc;

}
QMap<QString, int> ServeResponse::get_header(QJsonArray *columns){

    QMap<QString, int> header;

    for (int i = 0; i < columns->count(); ++i) {
        header.insert(columns->at(i).toString(), i);
    }

    return header;
}

QMap<QString, int> ServeResponse::get_header(QJsonObject *obj, QString defaultColumn) {

    QList<QString> list;
    QMap<QString, int> header;
    int index = 0;

    for (auto col = obj->constBegin(); col != obj->constEnd(); col++)
    {
        if (defaultColumn.isEmpty()){
            header.insert(col.key(), index);
            index++;
        }else{
            if (col.key() != defaultColumn)
                list.push_back(col.key());
            else
                list.insert(0, col.key());
        }
    }

    if (!list.empty()){
        for (int j = 0; j < list.count(); ++j) {
            header.insert(list.at(j), j);
        }
    }

    return header;
}

QSortFilterProxyModel * ServeResponse::get_proxyModel(QJsonArray &rows, QMap<QString, int> header) {

    if (rows.isEmpty())
        return nullptr;

    //QJsonArray array = doc.array();
    int rowCount = (int)rows.count();
    int colCount = (int)header.size();
    auto * model = new QStandardItemModel(rowCount, colCount);
    int index = 0;

    foreach ( const auto& Key, header.keys() )
    {
        const auto& Value = header[Key];
        model->setHorizontalHeaderItem(Value, new QStandardItem(Key));
    }

    index = 0;

    auto proxyModel = new QSortFilterProxyModel();

    for (auto it = rows.constBegin(); it != rows.constEnd(); ++it)
    {
        QJsonObject currentObject = it->toObject();
        for (auto col = currentObject.constBegin(); col != currentObject.constEnd(); ++col){
            const auto& indexCol = header[col.key()];;
            QStandardItem *itemVal = nullptr;
            if (col.value().isString())
                itemVal = new QStandardItem(col.value().toString());
            else if (col.value().isDouble())
                itemVal = new QStandardItem(QString::number(col.value().toInt()));
            else if (col.value().isBool())
                itemVal = new QStandardItem(QString::number(col.value().toBool()));
            model->setItem(index, indexCol, itemVal);

        }
        index++;
    }
    proxyModel->setSourceModel(model);

    return proxyModel;
}
