#include "serveresponse.h"

#ifndef QT_WEBSOCKETS_LIB
#include <iws_client.h>
#endif

#include <QStandardItemModel>
#include <QFileInfo>
#include <QDir>
#ifndef  QT_QML_CLIENT_APP
#include <QTableWidgetItem>
#endif

ServerResponse::ServerResponse(const QString& resp)
{
    isParse = false;
    parse(resp);
}

#ifndef QT_WEBSOCKETS_LIB
QString ServerResponse::base64_decode(const std::string &resp)
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

QString ServerResponse::base64_encode(const std::string &resp)
{
    std::string json;
    try {
        json = IClient::base64_encode(resp);
    }  catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    return QString::fromStdString(json);
}
#else
QString ServeResponse::base64_decode(const QByteArray &resp)
{

    return QByteArray::fromBase64(resp);
}

QString ServeResponse::base64_encode(const QByteArray &resp)
{
    return resp.toBase64();
}
#endif

void ServerResponse::parse(const QString& resp){


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
            auto _ip_address = obj.find("ip_address");
            auto _host_name = obj.find("host_name");
            auto _user_name = obj.find("user_name");

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
             if(_ip_address->isString()){
                 ip_address = _ip_address.value().toString();
             }
             if(_host_name->isString()){
                 host_name = _host_name.value().toString();
             }
             if(_user_name->isString()){
                 user_name = _user_name.value().toString();
             }
            isParse = true;
        }
    }

}

void ServerResponse::debugSaveResponse(const QString &filename, const QString &json)
{
    //ToDo: ?????????????????? ??????????????????
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

QString ServerResponse::to_string() const
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
    msg.insert("ip_address", ip_address);
    msg.insert("host_name", host_name);
    msg.insert("user_name", user_name);

    return QJsonDocument(msg).toJson(QJsonDocument::Indented);
}

QJsonDocument ServerResponse::parseResp(const QString &resp){

    QJsonDocument _doc(QJsonDocument::fromJson(resp.toUtf8()));
    return _doc;

}
QMap<QString, int> ServerResponse::get_header(QJsonArray *columns){

    QMap<QString, int> header;

    for (int i = 0; i < columns->count(); ++i) {
        header.insert(columns->at(i).toString(), i);
    }

    return header;
}

QMap<QString, int> ServerResponse::get_header(QJsonObject *obj, QString defaultColumn) {

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

QSortFilterProxyModel * ServerResponse::get_proxyModel(QJsonArray &rows, QMap<QString, int> header) {

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
#ifndef  QT_QML_CLIENT_APP
void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonObject& json) {

    table->setColumnCount(2);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem("????????????????"));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem("????????????????"));

    int index = 0;

    table->setRowCount((int)json.count());

    for (auto it = json.constBegin(); it != json.constEnd(); ++it)
    {
        auto itemKey = new QTableWidgetItem(it.key());
        itemKey->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
        table->setItem(index, 0, itemKey);

        QTableWidgetItem *itemVal = nullptr;
        if (it.value().isString())
            itemVal = new QTableWidgetItem(it.value().toString());
        else if (it.value().isDouble())
            itemVal = new QTableWidgetItem(QString::number(it.value().toInteger()));
        else if (it.value().isBool())
            itemVal = new QTableWidgetItem(QString::number(it.value().toBool()));

        if (itemVal){
            itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            table->setItem(index, 1, itemVal);
        }

        index++;
    }

}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonArray& array) {

    auto rowCount = array.count();
    table->setRowCount((int)rowCount);

    int colCount = 0;

    int index = 0;
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if(it->isObject()){
            QJsonObject currentObject = it->toObject();
            int indexCol = 0;
            if (index == 0){
                colCount = (int)currentObject.count();
                table->setColumnCount(colCount);
                for (auto col = currentObject.constBegin(); col != currentObject.constEnd(); ++col)
                {
                    table->setHorizontalHeaderItem(indexCol, new QTableWidgetItem(col.key()));
                    indexCol++;
                }
                table->setColumnCount(colCount);

            }else
            {
                if (colCount != currentObject.count() ){
                    table->clear();
                    table->setRowCount(0);
                    table->setColumnCount(0);
                    return;
                }
            }
            indexCol = 0;
            for (auto rowVal = currentObject.constBegin(); rowVal != currentObject.constEnd(); ++rowVal)
            {
                QTableWidgetItem *itemVal = nullptr;
                if (rowVal.value().isString())
                    itemVal = new QTableWidgetItem(rowVal.value().toString());
                else if (rowVal.value().isDouble())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toInteger()));
                else if (rowVal.value().isBool())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toBool()));

                if (itemVal){
                    itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    table->setItem(index, indexCol, itemVal);
                }
                indexCol++;
            }
            index++;
        }
    }

}

void ServeResponse::loadTableFromJson(QTableWidget *table, const QJsonArray& array, const QMap<QString, int>&header){

    auto rowCount = array.count();
    table->setRowCount((int)rowCount);
    table->setColumnCount((int)header.count());

    for (auto col = header.constBegin(); col != header.constEnd(); ++col)
    {
        const QString& key = col.key();
        table->setHorizontalHeaderItem(col.value(), new QTableWidgetItem(key));
    }

    int index = 0;
    for (auto it = array.constBegin(); it != array.constEnd(); ++it)
    {
        if(it->isObject()){
            QJsonObject currentObject = it->toObject();
            for (const auto& col : header.keys())
            {
                const QString& key = col;
                auto rowVal = currentObject.find(key);

                QTableWidgetItem *itemVal = nullptr;
                if (rowVal.value().isString())
                    itemVal = new QTableWidgetItem(rowVal.value().toString());
                else if (rowVal.value().isDouble())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toInteger()));
                else if (rowVal.value().isBool())
                    itemVal = new QTableWidgetItem(QString::number(rowVal.value().toBool()));

                if (itemVal){
                    itemVal->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
                    table->setItem(index, header[col], itemVal);
                }
            }

            index++;
        }
    }

}

#endif
